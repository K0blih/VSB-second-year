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


class InternalSourceHeader(BaseModel):
    x_internal_source: bool = Field(
        default=False,
        description="Whether the request simulates internal cloud traffic.",
    )


class FilePathParams(BaseModel):
    file_id: str = Field(
        ...,
        min_length=1,
        max_length=128,
        pattern=r"^[A-Za-z0-9-]+$",
        description="Unique file identifier.",
    )


class BucketPathParams(BaseModel):
    bucket_id: int = Field(..., gt=0, description="Bucket identifier.")


class BucketCreateRequest(BaseModel):
    name: str = Field(
        ...,
        min_length=3,
        max_length=63,
        pattern=r"^[a-z0-9][a-z0-9-]{1,61}[a-z0-9]$",
        description="Bucket name unique per user.",
        examples=["archive-2026"],
    )


class FileUploadForm(BaseModel):
    bucket_id: int = Field(..., gt=0, description="Target bucket identifier.")


class BucketResponse(BaseModel):
    model_config = ConfigDict(from_attributes=True)

    id: int = Field(..., gt=0)
    name: str = Field(..., min_length=3, max_length=63)
    created_at: datetime


class BucketBillingResponse(BucketResponse):
    bandwidth_bytes: int = Field(..., ge=0)
    current_storage_bytes: int = Field(..., ge=0)
    ingress_bytes: int = Field(..., ge=0)
    egress_bytes: int = Field(..., ge=0)
    internal_transfer_bytes: int = Field(..., ge=0)
    count_write_requests: int = Field(..., ge=0)
    count_read_requests: int = Field(..., ge=0)


class FileUploadResponse(BaseModel):
    model_config = ConfigDict(from_attributes=True)

    id: str = Field(..., min_length=1, max_length=128)
    bucket_id: int = Field(..., gt=0)
    filename: str = Field(..., min_length=1, max_length=255)
    size: int = Field(..., ge=0)


class FileMetadataResponse(FileUploadResponse):
    created_at: datetime
