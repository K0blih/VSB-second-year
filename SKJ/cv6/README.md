# Object Storage Service

FastAPI object storage service with bucket-based objects, Alembic migrations, advanced transfer billing, soft delete, and a browser UI.

## Setup

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
alembic upgrade head
uvicorn app.main:app --reload
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

`POST /files/upload` expects multipart form data with `bucket_id` and `file`.

## Frontend

Open `http://127.0.0.1:8000/` to use the browser UI. The health endpoint is available at `GET /health`.

## Implementation Notes

- File contents are stored on disk under `storage/<user-id>/`.
- File metadata is stored in `data/files.db`.
- Database schema changes are tracked in `alembic/versions/`.
- The first migration creates buckets and backfills existing `files` rows into a per-user default bucket so older data is preserved.
- JSON responses and validated API inputs are defined with Pydantic models in `app/schemas.py`.
- Local API docs are available at `http://127.0.0.1:8000/docs`.

## Examples

```bash
curl -X POST \
  -H "Content-Type: application/json" \
  -H "X-User-Id: user-1" \
  -d '{"name":"archive-2026"}' \
  http://127.0.0.1:8000/buckets/
```

```bash
curl -X POST \
  -H "X-User-Id: user-1" \
  -H "X-Internal-Source: false" \
  -F "bucket_id=1" \
  -F "file=@test.txt" \
  http://127.0.0.1:8000/files/upload
```

```bash
curl -H "X-User-Id: user-1" http://127.0.0.1:8000/buckets/1/billing/
```
