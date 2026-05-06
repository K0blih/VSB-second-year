# Object Storage Service

FastAPI object storage service with bucket-based objects, Alembic migrations, advanced transfer billing, soft delete, and a browser UI.

## Setup

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
alembic upgrade head
BROKER_WS_URL=ws://localhost:8000/broker uvicorn app.main:app --reload --port 8001
```

When you change the SQLAlchemy models, create and apply a migration with:

```bash
alembic revision --autogenerate -m "describe change"
alembic upgrade head
```

## API

All endpoints require the `X-User-Id` header.

Transfer-sensitive endpoints also accept `X-Internal-Source: true` to simulate internal cloud traffic.

- `POST /buckets/`
- `GET /buckets/`
- `GET /buckets/{bucket_id}/objects/`
- `GET /buckets/{bucket_id}/billing/`
- `POST /files/upload`
- `GET /files`
- `GET /files/{id}`
- `DELETE /files/{id}`
- `POST /buckets/{bucket_id}/objects/{file_id}/process`

`POST /files/upload` expects multipart form data with `bucket_id` and `file`.
`POST /buckets/{bucket_id}/objects/{file_id}/process` starts asynchronous image processing through the message broker and returns immediately.

## Frontend

Open `http://127.0.0.1:8001/` to use the browser UI. The health endpoint is available at `GET /health`.

## Implementation Notes

- File contents are stored on disk under `storage/<user-id>/`.
- File metadata is stored in `data/files.db`.
- Database schema changes are tracked in `alembic/versions/`.
- The first migration creates buckets and backfills existing `files` rows into a per-user default bucket so older data is preserved.
- JSON responses and validated API inputs are defined with Pydantic models in `app/schemas.py`.
- Local API docs are available at `http://127.0.0.1:8001/docs`.

## Examples

```bash
curl -X POST \
  -H "Content-Type: application/json" \
  -H "X-User-Id: user-1" \
  -d '{"name":"archive-2026"}' \
  http://127.0.0.1:8001/buckets/
```

```bash
curl -X POST \
  -H "X-User-Id: user-1" \
  -H "X-Internal-Source: false" \
  -F "bucket_id=1" \
  -F "file=@test.txt" \
  http://127.0.0.1:8001/files/upload
```

```bash
curl -H "X-User-Id: user-1" http://127.0.0.1:8001/buckets/1/billing/
```

Start the image worker in another terminal:

```bash
cd ../ImageWorker
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python worker.py --broker-url ws://localhost:8000/broker --s3-base-url http://localhost:8001
```

Start a processing job:

```bash
curl -X POST \
  -H "Content-Type: application/json" \
  -H "X-User-Id: user-1" \
  -d '{"operation":"grayscale","params":{}}' \
  http://127.0.0.1:8001/buckets/1/objects/YOUR_FILE_ID/process
```
