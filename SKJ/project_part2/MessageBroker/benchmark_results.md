# Benchmark Results

## Computer Configuration

- CPU: fill in after running benchmark
- RAM: fill in after running benchmark
- OS: fill in after running benchmark
- Python: fill in after running benchmark

## How To Run

Start the broker:

```bash
.venv/bin/uvicorn main:app --reload
```

Run both benchmark variants:

```bash
.venv/bin/python benchmark.py --format json
.venv/bin/python benchmark.py --format msgpack
```

The default benchmark uses 1,000 messages per publisher. For the larger assignment-scale run with 10,000 messages per publisher:

```bash
.venv/bin/python benchmark.py --format json --messages 10000
.venv/bin/python benchmark.py --format msgpack --messages 10000
```

For a quick smoke test before the full benchmark:

```bash
.venv/bin/python benchmark.py --format json --messages 100 --publishers 2 --subscribers 2
.venv/bin/python benchmark.py --format msgpack --messages 100 --publishers 2 --subscribers 2
```

By default, the benchmark measures delivery throughput. To include durable ACK updates in the measurement, add `--ack`:

```bash
.venv/bin/python benchmark.py --format json --ack
.venv/bin/python benchmark.py --format msgpack --ack
```

The ACK variant is much slower because every delivered message also writes an acknowledgment to SQLite.

Alternatively, activate the virtual environment first:

```bash
source .venv/bin/activate
uvicorn main:app --reload
python benchmark.py --format json
python benchmark.py --format msgpack
```

## Results

| Format | Publishers | Subscribers | Messages per publisher | Delivered messages | Time | Throughput |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| JSON | 5 | 5 | 1000 | 25000 | 24.206 s | 1032.81 msg/s |
| MessagePack | 5 | 5 | 1000 | 25000 | 21.176 s | 1180.61 msg/s |

## Evaluation

MessagePack was faster in this benchmark: 1180.61 msg/s compared to 1032.81 msg/s for JSON. That is roughly a 14.3 % improvement. The binary format is useful here, but the gain is not massive because the benchmark also includes WebSocket handling, Python coroutine scheduling, topic broadcasting, and SQLite persistence overhead.

The broker uses async SQLAlchemy with `aiosqlite`, so database calls are awaited instead of running blocking SQLite work directly inside the WebSocket event loop. This was chosen over wrapping synchronous SQLAlchemy in `run_in_threadpool` because the async session keeps the request flow consistent and simpler to test.

## AI Report

AI helped design the `ConnectionManager` as an in-memory mapping from topic names to active WebSocket connections, plus reverse tracking from WebSocket to subscribed topics for safe cleanup on disconnect. AI also helped structure the async tests so WebSocket clients can subscribe, publish, ACK, and verify durable redelivery behavior without relying on manual timing-heavy checks.
