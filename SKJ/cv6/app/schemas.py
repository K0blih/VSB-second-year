from datetime import datetime

from pydantic import BaseModel
from pydantic import ConfigDict


class FileUploadResponse(BaseModel):
    id: str
    filename: str
    size: int


class FileMetadataResponse(FileUploadResponse):
    model_config = ConfigDict(from_attributes=True)

    created_at: datetime
