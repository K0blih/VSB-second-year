from pathlib import Path
from uuid import uuid4

import aiofiles
from fastapi import UploadFile

from app.config import STORAGE_DIR


class StorageService:
    def __init__(self, base_dir: Path) -> None:
        self.base_dir = base_dir
        self.base_dir.mkdir(parents=True, exist_ok=True)

    def create_file_id(self) -> str:
        return str(uuid4())

    def get_user_dir(self, user_id: str) -> Path:
        user_dir = self.base_dir / user_id
        user_dir.mkdir(parents=True, exist_ok=True)
        return user_dir

    async def save_upload(self, user_id: str, file_id: str, upload: UploadFile) -> tuple[Path, int]:
        destination = self.get_user_dir(user_id) / file_id
        size = 0

        async with aiofiles.open(destination, "wb") as output_file:
            while chunk := await upload.read(1024 * 1024):
                size += len(chunk)
                await output_file.write(chunk)

        await upload.close()
        return destination, size

    def delete_file(self, path: str) -> None:
        file_path = Path(path)
        if file_path.exists():
            file_path.unlink()


storage_service = StorageService(STORAGE_DIR)
