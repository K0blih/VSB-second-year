# Object Storage Service

FastAPI object storage service for uploading, listing, downloading, and deleting files, with metadata persisted in SQLite through SQLAlchemy and validated/documented with Pydantic.

## Setup

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
uvicorn app.main:app --reload
```

## API

All file endpoints require the `X-User-Id` header.

## Frontend

Open `http://127.0.0.1:8000/` to use the browser UI. The health endpoint is available at `GET /health`.

- `POST /files/upload`
- `GET /files`
- `GET /files/{id}`
- `DELETE /files/{id}`

## Implementation Notes

- File contents are stored on disk under `storage/<user-id>/`.
- File metadata is stored in `data/files.db`.
- JSON responses and validated API inputs are defined with Pydantic models in [`app/schemas.py`].
- Local API docs are available at `http://127.0.0.1:8000/docs`.

## Examples

```bash
curl -X POST \
  -H "X-User-Id: user-1" \
  -F "file=@test.txt" \
  http://127.0.0.1:8000/files/upload
```

```bash
curl -H "X-User-Id: user-1" http://127.0.0.1:8000/files
```
