from __future__ import annotations

from typing import Any

from sqlalchemy import select, update
from sqlalchemy.ext.asyncio import AsyncSession, async_sessionmaker

from models import QueuedMessage
from protocol import WireFormat, deserialize_payload, serialize_payload


async def save_message(
    session_factory: async_sessionmaker[AsyncSession],
    topic: str,
    payload: Any,
    wire_format: WireFormat,
) -> QueuedMessage:
    async with session_factory() as session:
        message = QueuedMessage(
            topic=topic,
            payload=serialize_payload(payload, wire_format),
            format=wire_format,
            is_delivered=False,
        )
        session.add(message)
        await session.commit()
        await session.refresh(message)
        return message


async def undelivered_messages(
    session_factory: async_sessionmaker[AsyncSession],
    topic: str,
) -> list[QueuedMessage]:
    async with session_factory() as session:
        result = await session.execute(
            select(QueuedMessage)
            .where(QueuedMessage.topic == topic, QueuedMessage.is_delivered.is_(False))
            .order_by(QueuedMessage.created_at, QueuedMessage.id)
        )
        return list(result.scalars())


async def mark_delivered(
    session_factory: async_sessionmaker[AsyncSession],
    message_id: int,
) -> None:
    async with session_factory() as session:
        await session.execute(
            update(QueuedMessage)
            .where(QueuedMessage.id == message_id)
            .values(is_delivered=True)
        )
        await session.commit()


def delivery_payload(message: QueuedMessage) -> dict[str, Any]:
    return {
        "action": "deliver",
        "topic": message.topic,
        "message_id": message.id,
        "payload": deserialize_payload(message.payload, message.format),  # type: ignore[arg-type]
    }
