from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

import httpx

from config import VOLUME_DIR


@dataclass(frozen=True)
class CompactionResult:
    volume_id: int
    objects_rewritten: int
    original_size: int
    compacted_size: int


def volume_path(volume_id: int) -> Path:
    return VOLUME_DIR / f"volume_{volume_id}.dat"


def compact_volume(volume_id: int, s3_base_url: str) -> CompactionResult:
    source_path = volume_path(volume_id)
    compacted_path = VOLUME_DIR / f"volume_{volume_id}_compacted.dat"
    if not source_path.exists():
        raise FileNotFoundError(f"Volume does not exist: {source_path}")
    original_size = source_path.stat().st_size
    objects_rewritten = 0

    headers = {"X-Internal-Source": "true", "X-User-Id": "haystack-compactor"}
    with httpx.Client(base_url=s3_base_url, timeout=30.0, headers=headers) as client:
        response = client.get(f"/internal/volumes/{volume_id}/objects")
        response.raise_for_status()
        objects = response.json()

        with source_path.open("rb") as source, compacted_path.open("wb") as compacted:
            for item in objects:
                source.seek(int(item["offset"]))
                data = source.read(int(item["size"]))
                new_offset = compacted.tell()
                compacted.write(data)
                update = {
                    "volume_id": volume_id,
                    "offset": new_offset,
                    "size": len(data),
                }
                update_response = client.patch(
                    f"/internal/objects/{item['object_id']}/location",
                    json=update,
                )
                update_response.raise_for_status()
                objects_rewritten += 1

        compacted_size = compacted_path.stat().st_size

    source_path.unlink()
    compacted_path.rename(source_path)
    return CompactionResult(
        volume_id=volume_id,
        objects_rewritten=objects_rewritten,
        original_size=original_size,
        compacted_size=compacted_size,
    )


def main() -> None:
    parser = argparse.ArgumentParser(description="Compact one Haystack volume.")
    parser.add_argument("volume_id", type=int)
    parser.add_argument("--s3-base-url", default="http://localhost:8001")
    args = parser.parse_args()
    compact_volume(args.volume_id, args.s3_base_url)


if __name__ == "__main__":
    main()
