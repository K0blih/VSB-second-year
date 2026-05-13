from __future__ import annotations

import json
from typing import Any

import msgpack
import websockets
from websockets.exceptions import WebSocketException


def encode_message(message: dict[str, Any], wire_format: str) -> str | bytes:
    if wire_format == "msgpack":
        return msgpack.packb(message, use_bin_type=True)
    return json.dumps(message, ensure_ascii=False)


def decode_message(raw: str | bytes, wire_format: str) -> dict[str, Any]:
    if wire_format == "msgpack":
        decoded = msgpack.unpackb(raw if isinstance(raw, bytes) else raw.encode("latin1"), raw=False)
    else:
        decoded = json.loads(raw.decode("utf-8") if isinstance(raw, bytes) else raw)
    if not isinstance(decoded, dict):
        raise ValueError("Broker message must be an object.")
    return decoded


async def send_message(websocket: websockets.WebSocketClientProtocol, message: dict[str, Any], wire_format: str) -> None:
    await websocket.send(encode_message(message, wire_format))


async def receive_message(websocket: websockets.WebSocketClientProtocol, wire_format: str) -> dict[str, Any]:
    return decode_message(await websocket.recv(), wire_format)


async def publish_message(
    broker_url: str,
    topic: str,
    payload: dict[str, Any],
    *,
    wire_format: str = "json",
) -> None:
    try:
        async with websockets.connect(f"{broker_url}?format={wire_format}") as websocket:
            await send_message(
                websocket,
                {"action": "publish", "topic": topic, "payload": payload},
                wire_format,
            )
            await send_message(websocket, {"action": "flush"}, wire_format)
            while True:
                message = await receive_message(websocket, wire_format)
                if message == {"action": "flushed"}:
                    return
    except (OSError, TimeoutError, WebSocketException, json.JSONDecodeError, ValueError, msgpack.PackException, msgpack.UnpackException) as exc:
        raise ConnectionError("Failed to publish message to the broker.") from exc
