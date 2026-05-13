from __future__ import annotations

import asyncio
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class StoredNeedle:
    volume_id: int
    offset: int
    size: int


class VolumeStore:
    def __init__(self, volume_dir: Path, max_volume_size: int) -> None:
        self.volume_dir = volume_dir
        self.max_volume_size = max_volume_size
        self.current_volume_id = 1
        self.current_file = None
        self._lock = asyncio.Lock()

    def start(self) -> None:
        self.volume_dir.mkdir(parents=True, exist_ok=True)
        existing_ids = []
        for path in self.volume_dir.glob("volume_*.dat"):
            try:
                existing_ids.append(int(path.stem.removeprefix("volume_")))
            except ValueError:
                continue
        self.current_volume_id = max(existing_ids, default=1)
        self.current_file = self._open_current_file()

    def close(self) -> None:
        if self.current_file is not None:
            self.current_file.close()
            self.current_file = None

    def volume_path(self, volume_id: int) -> Path:
        return self.volume_dir / f"volume_{volume_id}.dat"

    def _open_current_file(self):
        handle = self.volume_path(self.current_volume_id).open("ab+")
        handle.seek(0, 2)
        return handle

    def _rotate(self) -> None:
        if self.current_file is not None:
            self.current_file.close()
        self.current_volume_id += 1
        self.current_file = self._open_current_file()

    async def rotate_if_current(self, volume_id: int) -> bool:
        async with self._lock:
            if self.current_file is None:
                self.start()
            if self.current_volume_id != volume_id:
                return False
            self._rotate()
            return True

    async def append(self, data: bytes) -> StoredNeedle:
        async with self._lock:
            if self.current_file is None:
                self.start()
            assert self.current_file is not None

            self.current_file.seek(0, 2)
            current_offset = self.current_file.tell()
            if current_offset > 0 and current_offset + len(data) > self.max_volume_size:
                self._rotate()
                assert self.current_file is not None
                current_offset = self.current_file.tell()

            self.current_file.write(data)
            self.current_file.flush()
            return StoredNeedle(
                volume_id=self.current_volume_id,
                offset=current_offset,
                size=len(data),
            )

    def read(self, volume_id: int, offset: int, size: int) -> bytes:
        path = self.volume_path(volume_id)
        with path.open("rb") as handle:
            handle.seek(offset)
            return handle.read(size)
