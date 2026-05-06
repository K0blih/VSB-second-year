# Async Pub/Sub Message Broker

Small FastAPI WebSocket message broker with topic subscriptions, publishing, delivery acknowledgements, queued undelivered messages, JSON/MessagePack formats, and a browser testing UI.

## Setup

Create and activate a virtual environment:

```bash
python3 -m venv .venv
source .venv/bin/activate
```

Install dependencies:

```bash
pip install -r requirements.txt
```

The default database is SQLite:

```text
broker.db
```

Tables are created automatically when the server starts. If you want to run the Alembic migration manually, use:

```bash
alembic upgrade head
```

## Start The Server

From the project directory, run:

```bash
.venv/bin/python -m uvicorn main:app --host 127.0.0.1 --port 8000
```

Then open:

```text
http://127.0.0.1:8000/broker
```

The `/broker` page contains a simple testing UI where you can connect, subscribe to a topic, publish messages, auto-ack deliveries, and inspect the event log.

## WebSocket Endpoint

JSON format:

```text
ws://127.0.0.1:8000/broker?format=json
```

MessagePack format:

```text
ws://127.0.0.1:8000/broker?format=msgpack
```

## CLI Examples

Run a subscriber:

```bash
.venv/bin/python mb_client.py subscriber --topic sensors --format json
```

Run a publisher in another terminal:

```bash
.venv/bin/python mb_client.py publisher --topic sensors --payload '{"temperature": 22.5}' --format json
```

Publish multiple messages:

```bash
.venv/bin/python mb_client.py publisher --topic sensors --payload '{"temperature": 22.5}' --count 5
```

## Run Tests

```bash
.venv/bin/python -m pytest
```

## Useful URLs

```text
http://127.0.0.1:8000/broker
http://127.0.0.1:8000/docs
http://127.0.0.1:8000/health
```
