from __future__ import annotations

from typing import Any

import msgpack
import websockets


def encode_message(message: dict[str, Any]) -> bytes:
    return msgpack.packb(message, use_bin_type=True)


def decode_message(raw: str | bytes) -> dict[str, Any]:
    if isinstance(raw, str):
        raw = raw.encode("latin1")
    decoded = msgpack.unpackb(raw, raw=False)
    if not isinstance(decoded, dict):
        raise ValueError("Broker message must be an object.")
    return decoded


async def send_message(websocket: websockets.WebSocketClientProtocol, message: dict[str, Any]) -> None:
    await websocket.send(encode_message(message))


async def receive_message(websocket: websockets.WebSocketClientProtocol) -> dict[str, Any]:
    return decode_message(await websocket.recv())

