# Object Storage Service

Simple FastAPI service for uploading, listing, downloading, and deleting files, with a built-in browser UI.

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
