from collections.abc import Iterable
from pathlib import Path
from typing import Annotated

from sqlalchemy.orm import Session
from pydantic import ValidationError

from fastapi import APIRouter, Depends, File, Form, HTTPException, Response, UploadFile, status
from fastapi.responses import FileResponse

from app.dependencies import (
    get_bucket_id,
    get_current_user_id,
    get_file_id,
    get_internal_source,
)
from app.database import get_db
from app.models import Bucket, File as FileModel
from app.schemas import (
    BucketBillingResponse,
    BucketCreateRequest,
    BucketResponse,
    FileMetadataResponse,
    FileUploadForm,
    FileUploadResponse,
)
from app.storage import storage_service


router = APIRouter()


def get_owned_bucket(db: Session, user_id: str, bucket_id: int) -> Bucket:
    bucket = db.get(Bucket, bucket_id)
    if bucket is None or bucket.owner_id != user_id:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Bucket not found.")
    return bucket


def get_owned_file(
    db: Session,
    user_id: str,
    file_id: str,
    *,
    include_deleted: bool = False,
) -> FileModel:
    file_record = db.get(FileModel, file_id)
    if file_record is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="File not found.")
    if file_record.user_id != user_id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Access denied.")
    if not include_deleted and file_record.is_deleted:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="File not found.")
    return file_record


def increment_request_counts(
    db: Session,
    bucket_ids: Iterable[int],
    *,
    read: int = 0,
    write: int = 0,
) -> None:
    seen_ids = {bucket_id for bucket_id in bucket_ids}
    if not seen_ids or (read == 0 and write == 0):
        return

    buckets = db.query(Bucket).filter(Bucket.id.in_(seen_ids)).all()
    for bucket in buckets:
        bucket.count_read_requests += read
        bucket.count_write_requests += write


def increment_request_count(
    db: Session,
    bucket_id: int | None,
    *,
    read: int = 0,
    write: int = 0,
) -> None:
    if bucket_id is None:
        return
    increment_request_counts(db, [bucket_id], read=read, write=write)


def apply_transfer_counters(bucket: Bucket, size: int, *, internal: bool, upload: bool) -> None:
    bucket.bandwidth_bytes += size
    if internal:
        bucket.internal_transfer_bytes += size
        return
    if upload:
        bucket.ingress_bytes += size
    else:
        bucket.egress_bytes += size


@router.post("/buckets/", response_model=BucketResponse, status_code=status.HTTP_201_CREATED, tags=["buckets"])
def create_bucket(
    payload: BucketCreateRequest,
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> BucketResponse:
    existing_bucket = (
        db.query(Bucket)
        .filter(Bucket.owner_id == user_id, Bucket.name == payload.name)
        .first()
    )
    if existing_bucket is not None:
        raise HTTPException(
            status_code=status.HTTP_409_CONFLICT,
            detail="Bucket with this name already exists.",
        )

    bucket = Bucket(owner_id=user_id, name=payload.name)
    db.add(bucket)
    db.flush()
    bucket.count_write_requests += 1
    db.commit()
    return bucket


@router.get("/buckets/", response_model=list[BucketResponse], tags=["buckets"])
def list_buckets(
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> list[BucketResponse]:
    return (
        db.query(Bucket)
        .filter(Bucket.owner_id == user_id)
        .order_by(Bucket.created_at.desc(), Bucket.id.desc())
        .all()
    )


@router.get(
    "/buckets/{bucket_id}/objects/",
    response_model=list[FileMetadataResponse],
    tags=["buckets"],
)
def list_bucket_objects(
    bucket_id: Annotated[int, Depends(get_bucket_id)],
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> list[FileMetadataResponse]:
    bucket = get_owned_bucket(db, user_id, bucket_id)
    files = (
        db.query(FileModel)
        .filter(
            FileModel.bucket_id == bucket.id,
            FileModel.user_id == user_id,
            FileModel.is_deleted.is_(False),
        )
        .order_by(FileModel.created_at.desc())
        .all()
    )
    bucket.count_read_requests += 1
    db.commit()
    return files


@router.get(
    "/buckets/{bucket_id}/billing/",
    response_model=BucketBillingResponse,
    tags=["buckets"],
)
def get_bucket_billing(
    bucket_id: Annotated[int, Depends(get_bucket_id)],
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> BucketBillingResponse:
    bucket = get_owned_bucket(db, user_id, bucket_id)
    bucket.count_read_requests += 1
    db.commit()
    return bucket


@router.post(
    "/files/upload",
    response_model=FileUploadResponse,
    status_code=status.HTTP_201_CREATED,
    tags=["files"],
)
async def upload_file(
    bucket_id: Annotated[int, Form(...)],
    file: UploadFile = File(...),
    user_id: str = Depends(get_current_user_id),
    internal_source: bool = Depends(get_internal_source),
    db: Session = Depends(get_db),
) -> FileUploadResponse:
    if not file.filename:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Uploaded file must have a filename.",
        )

    file_id = storage_service.create_file_id()
    saved_path = None
    try:
        payload = FileUploadForm(bucket_id=bucket_id)
    except ValidationError as exc:
        raise HTTPException(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            detail="Invalid bucket identifier format.",
        ) from exc
    bucket = get_owned_bucket(db, user_id, payload.bucket_id)

    try:
        saved_path, size = await storage_service.save_upload(user_id, file_id, file)
        file_record = FileModel(
            id=file_id,
            user_id=user_id,
            bucket_id=bucket.id,
            filename=file.filename,
            path=str(saved_path),
            size=size,
        )
        db.add(file_record)
        bucket.current_storage_bytes += size
        apply_transfer_counters(bucket, size, internal=internal_source, upload=True)
        increment_request_counts(db, [bucket.id], write=1)
        db.commit()
        return FileUploadResponse.model_validate(file_record)
    except Exception as exc:
        db.rollback()
        if saved_path is not None:
            storage_service.delete_file(str(saved_path))
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to store the uploaded file.",
        ) from exc


@router.get("/files", response_model=list[FileMetadataResponse], tags=["files"])
def list_files(
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> list[FileMetadataResponse]:
    files = (
        db.query(FileModel)
        .filter(FileModel.user_id == user_id, FileModel.is_deleted.is_(False))
        .order_by(FileModel.created_at.desc())
        .all()
    )
    increment_request_count(db, files[0].bucket_id if files else None, read=1)
    db.commit()
    return files


@router.get("/files/{file_id}", tags=["files"])
def download_file(
    file_id: Annotated[str, Depends(get_file_id)],
    user_id: str = Depends(get_current_user_id),
    internal_source: bool = Depends(get_internal_source),
    db: Session = Depends(get_db),
) -> FileResponse:
    file_record = get_owned_file(db, user_id, file_id)
    if not Path(file_record.path).exists():
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Stored file content is missing.",
        )
    bucket = get_owned_bucket(db, user_id, file_record.bucket_id)
    apply_transfer_counters(bucket, file_record.size, internal=internal_source, upload=False)
    increment_request_counts(db, [bucket.id], read=1)
    db.commit()
    return FileResponse(path=file_record.path, filename=file_record.filename)


@router.delete("/files/{file_id}", status_code=status.HTTP_204_NO_CONTENT, tags=["files"])
def delete_file(
    file_id: Annotated[str, Depends(get_file_id)],
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> Response:
    file_record = get_owned_file(db, user_id, file_id)
    bucket = get_owned_bucket(db, user_id, file_record.bucket_id)
    file_record.is_deleted = True
    increment_request_counts(db, [bucket.id], write=1)
    db.commit()
    return Response(status_code=status.HTTP_204_NO_CONTENT)
