from __future__ import annotations

import asyncio
import json

import msgpack
import websockets

from app.broker_client import receive_message, send_message
from app.config import BROKER_WS_URL
from app.database import SessionLocal
from app.models import Bucket, File


ACK_TOPIC = "storage.ack"


def apply_upload_billing(file_record: File, bucket: Bucket, size: int) -> None:
    bucket.current_storage_bytes += size
    bucket.bandwidth_bytes += size
    if file_record.upload_internal_source:
        bucket.internal_transfer_bytes += size
    else:
        bucket.ingress_bytes += size


def mark_file_ready(payload: dict) -> None:
    object_id = str(payload["object_id"])
    volume_id = int(payload["volume_id"])
    offset = int(payload["offset"])
    size = int(payload["size"])

    with SessionLocal() as db:
        file_record = db.get(File, object_id)
        if file_record is None or file_record.status == "ready" or file_record.is_deleted:
            return

        bucket = db.get(Bucket, file_record.bucket_id)
        if bucket is None:
            return

        file_record.volume_id = volume_id
        file_record.offset = offset
        file_record.size = size
        file_record.status = "ready"
        apply_upload_billing(file_record, bucket, size)
        db.commit()


async def listen_storage_acks() -> None:
    url = f"{BROKER_WS_URL}?format=msgpack"
    while True:
        try:
            async with websockets.connect(url) as websocket:
                await send_message(websocket, {"action": "subscribe", "topic": ACK_TOPIC}, "msgpack")
                while True:
                    message = await receive_message(websocket, "msgpack")
                    if message.get("action") != "deliver":
                        continue

                    payload = message.get("payload")
                    message_id = message.get("message_id")
                    if isinstance(payload, dict):
                        try:
                            mark_file_ready(payload)
                        finally:
                            if isinstance(message_id, int):
                                await send_message(websocket, {"action": "ack", "message_id": message_id}, "msgpack")
        except (
            OSError,
            websockets.WebSocketException,
            json.JSONDecodeError,
            ValueError,
            KeyError,
            TypeError,
            msgpack.UnpackException,
        ):
            await asyncio.sleep(1)
