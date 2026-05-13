from __future__ import annotations

import os
from pathlib import Path


BASE_DIR = Path(__file__).resolve().parent
VOLUME_DIR = Path(os.getenv("VOLUME_DIR", BASE_DIR / "volumes"))
MAX_VOLUME_SIZE = int(os.getenv("MAX_VOLUME_SIZE", str(100 * 1024 * 1024)))
BROKER_WS_URL = os.getenv("BROKER_WS_URL", "ws://localhost:8000/broker")

