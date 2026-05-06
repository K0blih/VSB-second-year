from __future__ import annotations

import json
from typing import Any, Literal

import msgpack
from pydantic import BaseModel, Field, ValidationError


WireFormat = Literal["json", "msgpack"]


class BrokerMessage(BaseModel):
    action: Literal["subscribe", "publish", "deliver", "ack", "flush"]
    topic: str | None = None
    payload: Any = None
    message_id: int | None = Field(default=None)


def normalize_format(value: str | None) -> WireFormat:
    if value in (None, "", "json"):
        return "json"
    if value == "msgpack":
        return "msgpack"
    raise ValueError("Unsupported format. Use 'json' or 'msgpack'.")


def encode_message(message: dict[str, Any], wire_format: WireFormat) -> str | bytes:
    if wire_format == "json":
        return json.dumps(message, ensure_ascii=False)
    return msgpack.packb(message, use_bin_type=True)


def decode_message(data: str | bytes, wire_format: WireFormat) -> dict[str, Any]:
    if wire_format == "json":
        if isinstance(data, bytes):
            data = data.decode("utf-8")
        decoded = json.loads(data)
    else:
        if isinstance(data, str):
            data = data.encode("latin1")
        decoded = msgpack.unpackb(data, raw=False)

    if not isinstance(decoded, dict):
        raise ValueError("Message must decode to an object.")
    return decoded


def validate_message(data: dict[str, Any]) -> BrokerMessage:
    try:
        return BrokerMessage.model_validate(data)
    except ValidationError as exc:
        raise ValueError(str(exc)) from exc


def serialize_payload(payload: Any, wire_format: WireFormat) -> bytes:
    if wire_format == "json":
        return json.dumps(payload, ensure_ascii=False).encode("utf-8")
    return msgpack.packb(payload, use_bin_type=True)


def deserialize_payload(payload: bytes, wire_format: WireFormat) -> Any:
    if wire_format == "json":
        return json.loads(payload.decode("utf-8"))
    return msgpack.unpackb(payload, raw=False)
