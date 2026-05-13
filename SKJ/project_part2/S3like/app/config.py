import os
from pathlib import Path


BASE_DIR = Path(__file__).resolve().parent.parent
DATA_DIR = Path(os.getenv("DATA_DIR", BASE_DIR / "data"))
DATABASE_URL = os.getenv("DATABASE_URL", f"sqlite:///{DATA_DIR / 'files.db'}")
BROKER_WS_URL = os.getenv("BROKER_WS_URL", "ws://localhost:8000/broker")
HAYSTACK_BASE_URL = os.getenv("HAYSTACK_BASE_URL", "http://localhost:8002")
S3_BASE_URL = os.getenv("S3_BASE_URL", "http://localhost:8001")
