from __future__ import annotations

import argparse
import asyncio
import json
import mimetypes
from dataclasses import dataclass
from io import BytesIO
from pathlib import Path
from typing import Any

import httpx
import numpy as np
from PIL import Image
import websockets


JOBS_TOPIC = "image.jobs"
DONE_TOPIC = "image.done"
SUPPORTED_OPERATIONS = {
    "negative",
    "invert",
    "inversion",
    "mirror",
    "horizontal_mirror",
    "flip",
    "crop",
    "brightness",
    "brighten",
    "grayscale",
}


class ImageProcessingError(ValueError):
    pass


@dataclass(frozen=True)
class WorkerConfig:
    broker_url: str = "ws://localhost:8000/broker"
    s3_base_url: str = "http://localhost:8001"


def apply_negative(img_array: np.ndarray) -> np.ndarray:
    return 255 - img_array


def apply_mirror(img_array: np.ndarray) -> np.ndarray:
    return img_array[:, ::-1, :]


def apply_crop(img_array: np.ndarray, params: dict[str, Any]) -> np.ndarray:
    height, width = img_array.shape[:2]

    has_explicit_size = {"x", "y"}.issubset(params) and (
        {"width", "height"}.issubset(params) or {"w", "h"}.issubset(params)
    )

    if has_explicit_size:
        try:
            x = int(params["x"])
            y = int(params["y"])
            crop_width = int(params.get("width", params.get("w")))
            crop_height = int(params.get("height", params.get("h")))
        except (TypeError, ValueError) as exc:
            raise ImageProcessingError("Crop parameters must be integers.") from exc
    else:
        try:
            border = int(params.get("border", 100))
        except (TypeError, ValueError) as exc:
            raise ImageProcessingError("Crop border must be an integer.") from exc
        x = border
        y = border
        crop_width = width - 2 * border
        crop_height = height - 2 * border

    if x < 0 or y < 0 or crop_width <= 0 or crop_height <= 0:
        raise ImageProcessingError("Crop bounds must be positive and inside the image.")
    if x + crop_width > width or y + crop_height > height:
        raise ImageProcessingError("Crop bounds exceed image dimensions.")

    return img_array[y : y + crop_height, x : x + crop_width, :]


def apply_brightness(img_array: np.ndarray, params: dict[str, Any]) -> np.ndarray:
    try:
        amount = int(params.get("amount", 50))
    except (TypeError, ValueError) as exc:
        raise ImageProcessingError("Brightness amount must be an integer.") from exc

    adjusted = img_array.astype(np.int16) + amount
    return np.clip(adjusted, 0, 255).astype(np.uint8)


def apply_grayscale(img_array: np.ndarray) -> np.ndarray:
    red = img_array[:, :, 0].astype(np.float32)
    green = img_array[:, :, 1].astype(np.float32)
    blue = img_array[:, :, 2].astype(np.float32)
    gray = np.clip(0.299 * red + 0.587 * green + 0.114 * blue, 0, 255).astype(np.uint8)
    return np.stack((gray, gray, gray), axis=-1)


def process_array(operation: str, img_array: np.ndarray, params: dict[str, Any] | None = None) -> np.ndarray:
    params = params or {}
    normalized_operation = operation.lower()

    if normalized_operation in {"negative", "invert", "inversion"}:
        return apply_negative(img_array)
    if normalized_operation in {"mirror", "horizontal_mirror", "flip"}:
        return apply_mirror(img_array)
    if normalized_operation == "crop":
        return apply_crop(img_array, params)
    if normalized_operation in {"brightness", "brighten"}:
        return apply_brightness(img_array, params)
    if normalized_operation == "grayscale":
        return apply_grayscale(img_array)

    raise ImageProcessingError(f"Unsupported operation: {operation}")


def validate_operation(operation: str) -> None:
    if operation.lower() not in SUPPORTED_OPERATIONS:
        raise ImageProcessingError(f"Unsupported operation: {operation}")


def output_filename(filename: str, operation: str) -> str:
    path = Path(filename)
    suffix = path.suffix or ".png"
    stem = path.stem or "image"
    return f"{stem}-{operation.lower()}{suffix}"


def image_format_for(filename: str) -> str:
    suffix = Path(filename).suffix.lower()
    if suffix in {".jpg", ".jpeg"}:
        return "JPEG"
    if suffix == ".webp":
        return "WEBP"
    return "PNG"


def process_image_bytes(source: bytes, filename: str, operation: str, params: dict[str, Any]) -> tuple[bytes, str]:
    with Image.open(BytesIO(source)) as image:
        rgb_image = image.convert("RGB")
        img_array = np.array(rgb_image)

    processed = process_array(operation, img_array, params)
    output = Image.fromarray(processed)
    output_name = output_filename(filename, operation)
    output_format = image_format_for(output_name)
    if output_format == "JPEG" and output.mode != "RGB":
        output = output.convert("RGB")

    buffer = BytesIO()
    output.save(buffer, format=output_format)
    return buffer.getvalue(), output_name


async def send_json(websocket: websockets.WebSocketClientProtocol, message: dict[str, Any]) -> None:
    await websocket.send(json.dumps(message, ensure_ascii=False))


async def receive_json(websocket: websockets.WebSocketClientProtocol) -> dict[str, Any]:
    raw = await websocket.recv()
    if isinstance(raw, bytes):
        raw = raw.decode("utf-8")
    decoded = json.loads(raw)
    if not isinstance(decoded, dict):
        raise ValueError("Broker message must be an object.")
    return decoded


async def publish_done(websocket: websockets.WebSocketClientProtocol, payload: dict[str, Any]) -> None:
    await send_json(websocket, {"action": "publish", "topic": DONE_TOPIC, "payload": payload})


async def handle_job(payload: dict[str, Any], config: WorkerConfig) -> dict[str, Any]:
    job_id = str(payload.get("job_id", ""))
    user_id = str(payload["user_id"])
    bucket_id = int(payload["bucket_id"])
    file_id = str(payload["file_id"])
    filename = str(payload.get("filename") or file_id)
    operation = str(payload["operation"])
    params = payload.get("params") or {}
    if not isinstance(params, dict):
        raise ImageProcessingError("Job params must be an object.")
    validate_operation(operation)

    headers = {"X-User-Id": user_id, "X-Internal-Source": "true"}
    async with httpx.AsyncClient(base_url=config.s3_base_url, timeout=30.0) as client:
        download_response = await client.get(f"/files/{file_id}", headers=headers)
        download_response.raise_for_status()

        processed_bytes, processed_filename = process_image_bytes(
            download_response.content,
            filename,
            operation,
            params,
        )
        content_type = mimetypes.guess_type(processed_filename)[0] or "application/octet-stream"
        upload_response = await client.post(
            "/files/upload",
            headers=headers,
            data={"bucket_id": str(bucket_id)},
            files={"file": (processed_filename, processed_bytes, content_type)},
        )
        upload_response.raise_for_status()

    return {
        "job_id": job_id,
        "status": "completed",
        "operation": operation,
        "source_file_id": file_id,
        "result": upload_response.json(),
    }


async def run_worker(config: WorkerConfig) -> None:
    url = f"{config.broker_url}?format=json"
    while True:
        try:
            async with websockets.connect(url) as websocket:
                await send_json(websocket, {"action": "subscribe", "topic": JOBS_TOPIC})
                while True:
                    message = await receive_json(websocket)
                    if message.get("action") != "deliver":
                        continue

                    payload = message.get("payload")
                    message_id = message.get("message_id")
                    if not isinstance(payload, dict):
                        done_payload = {
                            "job_id": "",
                            "status": "failed",
                            "error": "Job payload must be an object.",
                        }
                    else:
                        try:
                            done_payload = await handle_job(payload, config)
                        except Exception as exc:
                            done_payload = {
                                "job_id": str(payload.get("job_id", "")),
                                "status": "failed",
                                "operation": str(payload.get("operation", "")),
                                "source_file_id": str(payload.get("file_id", "")),
                                "error": str(exc),
                            }

                    await publish_done(websocket, done_payload)
                    if isinstance(message_id, int):
                        await send_json(websocket, {"action": "ack", "message_id": message_id})
        except (OSError, websockets.WebSocketException, json.JSONDecodeError, ValueError):
            await asyncio.sleep(1)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Async image processing worker")
    parser.add_argument("--broker-url", default="ws://localhost:8000/broker")
    parser.add_argument("--s3-base-url", default="http://localhost:8001")
    return parser


async def main() -> None:
    args = build_parser().parse_args()
    await run_worker(WorkerConfig(broker_url=args.broker_url, s3_base_url=args.s3_base_url))


if __name__ == "__main__":
    asyncio.run(main())
