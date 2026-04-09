from typing import Annotated

from fastapi import Header, HTTPException, Path, status
from pydantic import ValidationError

from app.schemas import BucketPathParams, FilePathParams, InternalSourceHeader, UserHeader


def get_current_user_id(
    x_user_id: Annotated[str | None, Header(alias="X-User-Id")] = None,
) -> str:
    if x_user_id is None:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="X-User-Id header is required.",
        )
    try:
        return UserHeader(x_user_id=x_user_id).x_user_id
    except ValidationError as exc:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="X-User-Id may only contain letters, numbers, underscores, and hyphens.",
        ) from exc


def get_file_id(
    file_id: Annotated[str, Path(description="Unique file identifier.")],
) -> str:
    try:
        return FilePathParams(file_id=file_id).file_id
    except ValidationError as exc:
        raise HTTPException(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            detail="Invalid file identifier format.",
        ) from exc


def get_bucket_id(
    bucket_id: Annotated[int, Path(description="Bucket identifier.")],
) -> int:
    try:
        return BucketPathParams(bucket_id=bucket_id).bucket_id
    except ValidationError as exc:
        raise HTTPException(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            detail="Invalid bucket identifier format.",
        ) from exc


def get_internal_source(
    x_internal_source: Annotated[str | None, Header(alias="X-Internal-Source")] = None,
) -> bool:
    try:
        return InternalSourceHeader(x_internal_source=x_internal_source or False).x_internal_source
    except ValidationError as exc:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="X-Internal-Source must be a boolean value.",
        ) from exc
