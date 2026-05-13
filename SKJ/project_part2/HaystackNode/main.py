from __future__ import annotations

import asyncio

import msgpack
import websockets
from fastapi import FastAPI, HTTPException, Response, status

from broker_client import receive_message, send_message
from config import BROKER_WS_URL, MAX_VOLUME_SIZE, VOLUME_DIR
from compact import compact_volume
from storage import VolumeStore


WRITE_TOPIC = "storage.write"
ACK_TOPIC = "storage.ack"

app = FastAPI(title="Haystack Storage Node")
volume_store = VolumeStore(VOLUME_DIR, MAX_VOLUME_SIZE)


async def publish_ack(websocket: websockets.WebSocketClientProtocol, payload: dict) -> None:
    await send_message(websocket, {"action": "publish", "topic": ACK_TOPIC, "payload": payload})


async def handle_write(payload: dict) -> dict:
    object_id = str(payload["object_id"])
    data = payload["data"]
    if not isinstance(data, bytes):
        raise ValueError("storage.write payload data must be bytes.")

    stored = await volume_store.append(data)
    return {
        "object_id": object_id,
        "volume_id": stored.volume_id,
        "offset": stored.offset,
        "size": stored.size,
    }


async def listen_storage_writes() -> None:
    url = f"{BROKER_WS_URL}?format=msgpack"
    while True:
        try:
            async with websockets.connect(url) as websocket:
                await send_message(websocket, {"action": "subscribe", "topic": WRITE_TOPIC})
                while True:
                    message = await receive_message(websocket)
                    if message.get("action") != "deliver":
                        continue

                    message_id = message.get("message_id")
                    payload = message.get("payload")
                    if isinstance(payload, dict):
                        ack_payload = await handle_write(payload)
                        await publish_ack(websocket, ack_payload)

                    if isinstance(message_id, int):
                        await send_message(websocket, {"action": "ack", "message_id": message_id})
        except (
            OSError,
            websockets.WebSocketException,
            ValueError,
            KeyError,
            TypeError,
            msgpack.UnpackException,
        ):
            await asyncio.sleep(1)


@app.on_event("startup")
async def startup() -> None:
    volume_store.start()
    asyncio.create_task(listen_storage_writes())


@app.on_event("shutdown")
async def shutdown() -> None:
    volume_store.close()


@app.get("/health")
def health() -> dict[str, str]:
    return {"status": "ok"}


@app.get("/volume/{volume_id}/{offset}/{size}")
def read_volume(volume_id: int, offset: int, size: int) -> Response:
    if volume_id <= 0 or offset < 0 or size < 0:
        raise HTTPException(status_code=status.HTTP_422_UNPROCESSABLE_ENTITY, detail="Invalid volume range.")

    try:
        data = volume_store.read(volume_id, offset, size)
    except FileNotFoundError as exc:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Volume not found.") from exc

    if len(data) != size:
        raise HTTPException(status_code=status.HTTP_416_REQUESTED_RANGE_NOT_SATISFIABLE, detail="Range not available.")
    return Response(content=data, media_type="application/octet-stream")


@app.post("/admin/volumes/{volume_id}/compact")
async def compact_volume_endpoint(volume_id: int, s3_base_url: str = "http://localhost:8001") -> dict[str, int | bool | str]:
    if volume_id <= 0:
        raise HTTPException(status_code=status.HTTP_422_UNPROCESSABLE_ENTITY, detail="Invalid volume identifier.")

    rotated_active_volume = await volume_store.rotate_if_current(volume_id)
    try:
        result = await asyncio.to_thread(compact_volume, volume_id, s3_base_url)
    except FileNotFoundError as exc:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Volume not found.") from exc
    except Exception as exc:
        raise HTTPException(status_code=status.HTTP_502_BAD_GATEWAY, detail="Volume compaction failed.") from exc

    return {
        "status": "compacted",
        "volume_id": result.volume_id,
        "rotated_active_volume": rotated_active_volume,
        "objects_rewritten": result.objects_rewritten,
        "original_size": result.original_size,
        "compacted_size": result.compacted_size,
    }
