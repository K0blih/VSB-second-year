from uuid import uuid4
from collections.abc import Iterable
from typing import Annotated

import httpx
from sqlalchemy.orm import Session
from pydantic import ValidationError

from fastapi import APIRouter, Depends, File, Form, HTTPException, Response, UploadFile, status

from app.broker_client import publish_message
from app.config import BROKER_WS_URL, HAYSTACK_BASE_URL, S3_BASE_URL
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
    ImageProcessRequest,
    ImageProcessResponse,
    ObjectLocationUpdate,
    VolumeObjectResponse,
    FileUploadForm,
    FileUploadResponse,
)


router = APIRouter()


async def publish_broker_message(topic: str, payload: dict) -> None:
    try:
        await publish_message(BROKER_WS_URL, topic, payload, wire_format="json")
    except ConnectionError as exc:
        raise HTTPException(
            status_code=status.HTTP_502_BAD_GATEWAY,
            detail="Failed to publish image processing job to the broker.",
        ) from exc


async def publish_storage_write(payload: dict) -> None:
    try:
        await publish_message(BROKER_WS_URL, "storage.write", payload, wire_format="msgpack")
    except ConnectionError as exc:
        raise HTTPException(
            status_code=status.HTTP_502_BAD_GATEWAY,
            detail="Failed to publish storage write to the broker.",
        ) from exc


def remove_pending_upload(db: Session, file_id: str) -> None:
    stored_record = db.get(FileModel, file_id)
    if stored_record is None or stored_record.status != "uploading":
        return

    bucket = db.get(Bucket, stored_record.bucket_id)
    if bucket is not None and bucket.count_write_requests > 0:
        bucket.count_write_requests -= 1
    db.delete(stored_record)
    db.commit()


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
    "/buckets/{bucket_id}/objects/{file_id}/process",
    response_model=ImageProcessResponse,
    tags=["images"],
)
async def process_bucket_object(
    payload: ImageProcessRequest,
    bucket_id: Annotated[int, Depends(get_bucket_id)],
    file_id: Annotated[str, Depends(get_file_id)],
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> ImageProcessResponse:
    bucket = get_owned_bucket(db, user_id, bucket_id)
    file_record = get_owned_file(db, user_id, file_id)
    if file_record.bucket_id != bucket.id:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="File not found in bucket.")
    if file_record.status != "ready":
        raise HTTPException(
            status_code=status.HTTP_409_CONFLICT,
            detail="File upload is still being finalized.",
        )

    job_id = str(uuid4())
    await publish_broker_message(
        "image.jobs",
        {
            "job_id": job_id,
            "user_id": user_id,
            "bucket_id": bucket.id,
            "file_id": file_record.id,
            "filename": file_record.filename,
            "operation": payload.operation,
            "params": payload.params,
        },
    )
    return ImageProcessResponse(status="processing_started", job_id=job_id)


@router.post(
    "/files/upload",
    response_model=FileUploadResponse,
    status_code=status.HTTP_202_ACCEPTED,
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

    file_id = str(uuid4())
    try:
        payload = FileUploadForm(bucket_id=bucket_id)
    except ValidationError as exc:
        raise HTTPException(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            detail="Invalid bucket identifier format.",
        ) from exc
    bucket = get_owned_bucket(db, user_id, payload.bucket_id)

    try:
        data = await file.read()
        await file.close()
        file_record = FileModel(
            id=file_id,
            user_id=user_id,
            bucket_id=bucket.id,
            filename=file.filename,
            path=f"haystack://{file_id}",
            size=len(data),
            status="uploading",
            upload_internal_source=internal_source,
        )
        db.add(file_record)
        increment_request_counts(db, [bucket.id], write=1)
        db.commit()
        await publish_storage_write({"object_id": file_id, "data": data})
        return FileUploadResponse.model_validate(file_record)
    except HTTPException:
        db.rollback()
        remove_pending_upload(db, file_id)
        raise
    except Exception as exc:
        db.rollback()
        remove_pending_upload(db, file_id)
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
async def download_file(
    file_id: Annotated[str, Depends(get_file_id)],
    user_id: str = Depends(get_current_user_id),
    internal_source: bool = Depends(get_internal_source),
    db: Session = Depends(get_db),
) -> Response:
    file_record = get_owned_file(db, user_id, file_id)
    if file_record.status != "ready":
        raise HTTPException(
            status_code=status.HTTP_409_CONFLICT,
            detail="File upload is still being finalized.",
        )
    if file_record.volume_id is None or file_record.offset is None:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Stored file location is incomplete.",
        )
    bucket = get_owned_bucket(db, user_id, file_record.bucket_id)

    try:
        async with httpx.AsyncClient(base_url=HAYSTACK_BASE_URL, timeout=30.0) as client:
            haystack_response = await client.get(
                f"/volume/{file_record.volume_id}/{file_record.offset}/{file_record.size}"
            )
            haystack_response.raise_for_status()
    except httpx.HTTPStatusError as exc:
        raise HTTPException(
            status_code=status.HTTP_502_BAD_GATEWAY,
            detail="Haystack node could not read the stored file.",
        ) from exc
    except httpx.HTTPError as exc:
        raise HTTPException(
            status_code=status.HTTP_502_BAD_GATEWAY,
            detail="Haystack node is unavailable.",
        ) from exc

    apply_transfer_counters(bucket, file_record.size, internal=internal_source, upload=False)
    increment_request_counts(db, [bucket.id], read=1)
    db.commit()
    return Response(
        content=haystack_response.content,
        media_type=haystack_response.headers.get("content-type", "application/octet-stream"),
        headers={"Content-Disposition": f'attachment; filename="{file_record.filename}"'},
    )


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


@router.get(
    "/internal/volumes/{volume_id}/objects",
    response_model=list[VolumeObjectResponse],
    tags=["internal"],
)
def list_volume_objects(
    volume_id: int,
    internal_source: bool = Depends(get_internal_source),
    db: Session = Depends(get_db),
) -> list[VolumeObjectResponse]:
    if not internal_source:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Internal access required.")

    files = (
        db.query(FileModel)
        .filter(
            FileModel.volume_id == volume_id,
            FileModel.status == "ready",
            FileModel.is_deleted.is_(False),
            FileModel.offset.isnot(None),
        )
        .order_by(FileModel.offset.asc())
        .all()
    )
    return [
        VolumeObjectResponse(
            object_id=file_record.id,
            volume_id=file_record.volume_id,
            offset=file_record.offset,
            size=file_record.size,
        )
        for file_record in files
        if file_record.volume_id is not None and file_record.offset is not None
    ]


@router.patch("/internal/objects/{object_id}/location", tags=["internal"])
def update_object_location(
    object_id: str,
    payload: ObjectLocationUpdate,
    internal_source: bool = Depends(get_internal_source),
    db: Session = Depends(get_db),
) -> dict[str, str]:
    if not internal_source:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Internal access required.")

    file_record = db.get(FileModel, object_id)
    if file_record is None or file_record.is_deleted:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="File not found.")
    file_record.volume_id = payload.volume_id
    file_record.offset = payload.offset
    file_record.size = payload.size
    file_record.status = "ready"
    db.commit()
    return {"status": "updated"}


@router.post("/admin/volumes/{volume_id}/compact", tags=["admin"])
async def compact_haystack_volume(
    volume_id: int,
    internal_source: bool = Depends(get_internal_source),
) -> dict:
    if not internal_source:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Internal access required.")
    if volume_id <= 0:
        raise HTTPException(status_code=status.HTTP_422_UNPROCESSABLE_ENTITY, detail="Invalid volume identifier.")

    try:
        async with httpx.AsyncClient(base_url=HAYSTACK_BASE_URL, timeout=120.0) as client:
            response = await client.post(
                f"/admin/volumes/{volume_id}/compact",
                params={"s3_base_url": S3_BASE_URL},
            )
            response.raise_for_status()
            return response.json()
    except httpx.HTTPStatusError as exc:
        detail = "Haystack node could not compact the requested volume."
        try:
            payload = exc.response.json()
            detail = payload.get("detail", detail)
        except ValueError:
            pass
        raise HTTPException(status_code=exc.response.status_code, detail=detail) from exc
    except httpx.HTTPError as exc:
        raise HTTPException(
            status_code=status.HTTP_502_BAD_GATEWAY,
            detail="Haystack node is unavailable.",
        ) from exc
