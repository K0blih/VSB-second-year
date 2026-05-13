#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BROKER_DIR="$ROOT_DIR/MessageBroker"
S3_DIR="$ROOT_DIR/S3like"
HAYSTACK_DIR="$ROOT_DIR/HaystackNode"
IMAGE_WORKER_DIR="$ROOT_DIR/ImageWorker"

BROKER_URL="${BROKER_URL:-ws://127.0.0.1:8000/broker}"
S3_BASE_URL="${S3_BASE_URL:-http://127.0.0.1:8001}"
HAYSTACK_BASE_URL="${HAYSTACK_BASE_URL:-http://127.0.0.1:8002}"

PIDS=()

setup_service() {
  local service_dir="$1"
  if [[ ! -d "$service_dir/.venv" ]]; then
    python -m venv "$service_dir/.venv"
  fi
  "$service_dir/.venv/bin/pip" install -q -r "$service_dir/requirements.txt"
}

wait_for_http() {
  local url="$1"
  local name="$2"
  for _ in {1..60}; do
    if curl -fsS "$url" >/dev/null 2>&1; then
      return 0
    fi
    sleep 0.5
  done
  echo "Timed out waiting for $name at $url" >&2
  return 1
}

cleanup() {
  echo
  echo "Stopping services..."
  for pid in "${PIDS[@]}"; do
    if kill -0 "$pid" >/dev/null 2>&1; then
      kill "$pid" >/dev/null 2>&1 || true
    fi
  done
}
trap cleanup EXIT INT TERM

echo "Installing dependencies when needed..."
setup_service "$BROKER_DIR"
setup_service "$HAYSTACK_DIR"
setup_service "$S3_DIR"
setup_service "$IMAGE_WORKER_DIR"

echo "Applying S3 database migrations..."
(
  cd "$S3_DIR"
  .venv/bin/alembic upgrade head
)

echo "Starting MessageBroker on http://127.0.0.1:8000 ..."
(
  cd "$BROKER_DIR"
  .venv/bin/python -m uvicorn main:app --host 127.0.0.1 --port 8000
) &
PIDS+=("$!")
wait_for_http "http://127.0.0.1:8000/health" "MessageBroker"

echo "Starting HaystackNode on http://127.0.0.1:8002 ..."
(
  cd "$HAYSTACK_DIR"
  BROKER_WS_URL="$BROKER_URL" .venv/bin/python -m uvicorn main:app --host 127.0.0.1 --port 8002
) &
PIDS+=("$!")
wait_for_http "http://127.0.0.1:8002/health" "HaystackNode"

echo "Starting S3like on http://127.0.0.1:8001 ..."
(
  cd "$S3_DIR"
  BROKER_WS_URL="$BROKER_URL" HAYSTACK_BASE_URL="$HAYSTACK_BASE_URL" S3_BASE_URL="$S3_BASE_URL" .venv/bin/python -m uvicorn app.main:app --host 127.0.0.1 --port 8001
) &
PIDS+=("$!")
wait_for_http "http://127.0.0.1:8001/health" "S3like"

echo "Starting ImageWorker ..."
(
  cd "$IMAGE_WORKER_DIR"
  .venv/bin/python worker.py --broker-url "$BROKER_URL" --s3-base-url "$S3_BASE_URL"
) &
PIDS+=("$!")

cat <<EOF

Project Part 2 is running:
  MessageBroker:  http://127.0.0.1:8000
  S3 Gateway:     http://127.0.0.1:8001
  HaystackNode:   http://127.0.0.1:8002
  ImageWorker:    background worker

Open the S3 UI:
  http://127.0.0.1:8001/

Press Ctrl+C to stop all services.
EOF

wait
