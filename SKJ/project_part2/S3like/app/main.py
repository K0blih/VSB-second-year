from pathlib import Path

from fastapi import FastAPI
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles

from app.routes import router as files_router
from app.schemas import HealthResponse
from app.storage_ack import listen_storage_acks


app = FastAPI(title="Object Storage Service")
BASE_DIR = Path(__file__).resolve().parent.parent
STATIC_DIR = BASE_DIR / "static"


@app.on_event("startup")
async def start_storage_ack_listener() -> None:
    import asyncio

    asyncio.create_task(listen_storage_acks())


@app.get("/", response_class=HTMLResponse)
def index() -> str:
    return (STATIC_DIR / "index.html").read_text(encoding="utf-8")


@app.get("/health", response_model=HealthResponse)
def healthcheck() -> HealthResponse:
    return HealthResponse(status="ok")


app.mount("/static", StaticFiles(directory=STATIC_DIR), name="static")
app.include_router(files_router)
