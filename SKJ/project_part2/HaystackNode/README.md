# Haystack Storage Node

FastAPI service that stores binary objects in append-only `volume_*.dat` files.

## Run

```bash
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
BROKER_WS_URL=ws://localhost:8000/broker uvicorn main:app --reload --port 8002
```

Optional environment variables:

- `VOLUME_DIR`: directory for volume files, defaults to `./volumes`
- `MAX_VOLUME_SIZE`: max volume size in bytes, defaults to `104857600`
- `BROKER_WS_URL`: Message Broker WebSocket URL

