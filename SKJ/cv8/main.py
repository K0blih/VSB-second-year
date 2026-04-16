from __future__ import annotations

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse, RedirectResponse

from connection_manager import ConnectionManager
from database import SessionLocal, init_db
from protocol import decode_message, normalize_format, validate_message
from repository import delivery_payload, mark_delivered, save_message, undelivered_messages


app = FastAPI(title="Async Pub/Sub Message Broker")
manager = ConnectionManager()


@app.get("/")
async def root() -> RedirectResponse:
    return RedirectResponse(url="/broker")


@app.get("/broker", response_class=HTMLResponse)
async def broker_info() -> str:
    return """
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>Async Pub/Sub Message Broker</title>
  <style>
    body { font-family: system-ui, sans-serif; max-width: 760px; margin: 40px auto; line-height: 1.5; padding: 0 20px; }
    code, pre { background: #f4f4f4; border-radius: 6px; }
    code { padding: 2px 5px; }
    pre { padding: 14px; overflow-x: auto; }
  </style>
</head>
<body>
  <h1>Async Pub/Sub Message Broker</h1>
  <p>
    This path is also the WebSocket endpoint. Browser address bars send normal HTTP requests,
    so this page is shown for <code>GET /broker</code>.
  </p>
  <p>WebSocket URL:</p>
  <pre>ws://localhost:8000/broker?format=json</pre>
  <p>Run a subscriber:</p>
  <pre>.venv/bin/python mb_client.py subscriber --topic sensors --format json</pre>
  <p>Run a publisher in another terminal:</p>
  <pre>.venv/bin/python mb_client.py publisher --topic sensors --payload '{"temperature": 22.5}' --format json</pre>
  <p>Other useful pages: <a href="/docs">/docs</a> and <a href="/health">/health</a>.</p>
</body>
</html>
"""


@app.get("/health")
async def health() -> dict[str, str]:
    return {"status": "ok"}


@app.on_event("startup")
async def startup() -> None:
    await init_db()


@app.websocket("/broker")
async def broker(websocket: WebSocket, format: str = "json") -> None:
    try:
        wire_format = normalize_format(format)
    except ValueError:
        await websocket.close(code=1003)
        return

    await manager.connect(websocket, wire_format)

    try:
        while True:
            raw = await websocket.receive()
            if "text" in raw and raw["text"] is not None:
                decoded = decode_message(raw["text"], wire_format)
            elif "bytes" in raw and raw["bytes"] is not None:
                decoded = decode_message(raw["bytes"], wire_format)
            else:
                continue

            message = validate_message(decoded)

            if message.action == "subscribe":
                if not message.topic:
                    await manager.send(websocket, {"action": "error", "error": "Missing topic."})
                    continue
                await manager.subscribe(websocket, message.topic)
                historical = await undelivered_messages(SessionLocal, message.topic)
                for queued in historical:
                    await manager.send(websocket, delivery_payload(queued))
                await manager.send(websocket, {"action": "subscribed", "topic": message.topic})

            elif message.action == "publish":
                if not message.topic:
                    await manager.send(websocket, {"action": "error", "error": "Missing topic."})
                    continue
                queued = await save_message(SessionLocal, message.topic, message.payload, wire_format)
                await manager.broadcast(message.topic, delivery_payload(queued))

            elif message.action == "ack":
                if message.message_id is None:
                    await manager.send(websocket, {"action": "error", "error": "Missing message_id."})
                    continue
                await mark_delivered(SessionLocal, message.message_id)

            elif message.action == "flush":
                await manager.send(websocket, {"action": "flushed"})

    except (WebSocketDisconnect, ValueError):
        pass
    finally:
        await manager.disconnect(websocket)
