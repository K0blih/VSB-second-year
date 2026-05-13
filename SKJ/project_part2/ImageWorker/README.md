# Image Processing Worker

Async worker for the event-driven image processing assignment. It subscribes to `image.jobs`, downloads the source image from the S3-like service, applies a NumPy operation, uploads the processed image as a new object, and publishes `image.done`.

All commands below are meant to be run from the project root:

```bash
pwd
# /home/richard-chovanec/Documents/VSB-second-year/SKJ/project
```

## Setup

```bash
python3 -m venv ImageWorker/.venv
ImageWorker/.venv/bin/pip install -r ImageWorker/requirements.txt
```

If the broker or S3-like service dependencies are not installed yet, install them too:

```bash
python3 -m venv MessageBroker/.venv
MessageBroker/.venv/bin/pip install -r MessageBroker/requirements.txt

python3 -m venv S3like/.venv
S3like/.venv/bin/pip install -r S3like/requirements.txt
```

## Run

Start these in three separate terminals, from the project root.

Terminal 1, message broker:

```bash
cd MessageBroker
.venv/bin/python -m uvicorn main:app --reload --port 8000
```

Terminal 2, S3-like server:

```bash
cd S3like
.venv/bin/alembic upgrade head
BROKER_WS_URL=ws://localhost:8000/broker .venv/bin/python -m uvicorn app.main:app --reload --port 8001
```

Terminal 3, image worker:

```bash
ImageWorker/.venv/bin/python ImageWorker/worker.py --broker-url ws://localhost:8000/broker --s3-base-url http://localhost:8001
```

Open the S3 browser UI at:

```text
http://localhost:8001/
```

## Operations

Supported operations are `negative`, `mirror`, `crop`, `brightness`, and `grayscale`.

Crop accepts either `{"x":0,"y":0,"width":200,"height":200}`, the shorter `{"x":0,"y":0,"w":200,"h":200}`, or `{"border":100}`. Brightness accepts `{"amount":50}`.

## Tests

```bash
ImageWorker/.venv/bin/pytest ImageWorker/tests -q
```
