from datetime import datetime

from pydantic import BaseModel, ConfigDict, Field


class HealthResponse(BaseModel):
    status: str = Field(
        ...,
        min_length=2,
        max_length=20,
        description="Application health state.",
        examples=["ok"],
    )


class UserHeader(BaseModel):
    x_user_id: str = Field(
        ...,
        min_length=1,
        max_length=64,
        pattern=r"^[A-Za-z0-9_-]+$",
        description="Tenant identifier passed in the X-User-Id header.",
        examples=["user-1"],
    )


class FilePathParams(BaseModel):
    file_id: str = Field(
        ...,
        min_length=1,
        max_length=128,
        pattern=r"^[A-Za-z0-9-]+$",
        description="Unique file identifier.",
    )


class FileUploadResponse(BaseModel):
    model_config = ConfigDict(from_attributes=True)

    id: str = Field(..., min_length=1, max_length=128)
    filename: str = Field(..., min_length=1, max_length=255)
    size: int = Field(..., ge=0)


class FileMetadataResponse(FileUploadResponse):
    created_at: datetime
