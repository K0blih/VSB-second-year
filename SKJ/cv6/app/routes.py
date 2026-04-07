from pathlib import Path
from typing import Annotated

from sqlalchemy.orm import Session

from fastapi import APIRouter, Depends, File, HTTPException, Response, UploadFile, status
from fastapi.responses import FileResponse

from app.dependencies import get_current_user_id, get_file_id
from app.database import get_db
from app.models import File as FileModel
from app.schemas import FileMetadataResponse, FileUploadResponse
from app.storage import storage_service


router = APIRouter(prefix="/files", tags=["files"])


def get_owned_file(db: Session, user_id: str, file_id: str) -> FileModel:
    file_record = db.get(FileModel, file_id)
    if file_record is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="File not found.")
    if file_record.user_id != user_id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Access denied.")
    return file_record


@router.post("/upload", response_model=FileUploadResponse, status_code=status.HTTP_201_CREATED)
async def upload_file(
    file: UploadFile = File(...),
    user_id: str = Depends(get_current_user_id),
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
        saved_path, size = await storage_service.save_upload(user_id, file_id, file)
        file_record = FileModel(
            id=file_id,
            user_id=user_id,
            filename=file.filename,
            path=str(saved_path),
            size=size,
        )
        db.add(file_record)
        db.commit()
        db.refresh(file_record)
        return FileUploadResponse.model_validate(file_record)
    except Exception as exc:
        db.rollback()
        if saved_path is not None:
            storage_service.delete_file(str(saved_path))
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to store the uploaded file.",
        ) from exc


@router.get("", response_model=list[FileMetadataResponse])
def list_files(
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> list[FileMetadataResponse]:
    files = (
        db.query(FileModel)
        .filter(FileModel.user_id == user_id)
        .order_by(FileModel.created_at.desc())
        .all()
    )
    return files


@router.get("/{file_id}")
def download_file(
    file_id: Annotated[str, Depends(get_file_id)],
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> FileResponse:
    file_record = get_owned_file(db, user_id, file_id)
    if not Path(file_record.path).exists():
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Stored file content is missing.",
        )
    return FileResponse(path=file_record.path, filename=file_record.filename)


@router.delete("/{file_id}", status_code=status.HTTP_204_NO_CONTENT)
def delete_file(
    file_id: Annotated[str, Depends(get_file_id)],
    user_id: str = Depends(get_current_user_id),
    db: Session = Depends(get_db),
) -> Response:
    file_record = get_owned_file(db, user_id, file_id)
    storage_service.delete_file(file_record.path)
    db.delete(file_record)
    db.commit()
    return Response(status_code=status.HTTP_204_NO_CONTENT)
