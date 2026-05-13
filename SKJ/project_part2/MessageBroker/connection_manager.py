from __future__ import annotations

import asyncio
from collections import defaultdict

from fastapi import WebSocket

from protocol import WireFormat, encode_message


class ConnectionManager:
    def __init__(self) -> None:
        self.active_connections: dict[str, set[WebSocket]] = defaultdict(set)
        self.websocket_topics: dict[WebSocket, set[str]] = defaultdict(set)
        self.websocket_formats: dict[WebSocket, WireFormat] = {}
        self._lock = asyncio.Lock()

    async def connect(self, websocket: WebSocket, wire_format: WireFormat) -> None:
        await websocket.accept()
        async with self._lock:
            self.websocket_formats[websocket] = wire_format

    async def subscribe(self, websocket: WebSocket, topic: str) -> None:
        async with self._lock:
            self.active_connections[topic].add(websocket)
            self.websocket_topics[websocket].add(topic)

    async def disconnect(self, websocket: WebSocket) -> None:
        async with self._lock:
            topics = self.websocket_topics.pop(websocket, set())
            for topic in topics:
                self.active_connections[topic].discard(websocket)
                if not self.active_connections[topic]:
                    del self.active_connections[topic]
            self.websocket_formats.pop(websocket, None)

    async def send(self, websocket: WebSocket, message: dict) -> None:
        wire_format = self.websocket_formats.get(websocket, "json")
        encoded = encode_message(message, wire_format)
        if wire_format == "json":
            await websocket.send_text(encoded)  # type: ignore[arg-type]
        else:
            await websocket.send_bytes(encoded)  # type: ignore[arg-type]

    async def broadcast(self, topic: str, message: dict) -> None:
        async with self._lock:
            subscribers = list(self.active_connections.get(topic, set()))

        stale: list[WebSocket] = []
        for websocket in subscribers:
            try:
                await self.send(websocket, message)
            except RuntimeError:
                stale.append(websocket)

        for websocket in stale:
            await self.disconnect(websocket)
