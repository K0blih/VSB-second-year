from __future__ import annotations

import argparse
import asyncio
import contextlib
from datetime import datetime
import time

import websockets
from websockets.exceptions import ConnectionClosed

from protocol import WireFormat, decode_message, encode_message


async def send_message(websocket: websockets.WebSocketClientProtocol, message: dict, wire_format: WireFormat) -> None:
    await websocket.send(encode_message(message, wire_format))


async def subscriber(
    subscriber_id: int,
    url: str,
    topic: str,
    wire_format: WireFormat,
    target_messages: int,
    counter: asyncio.Queue[int],
    ready: asyncio.Queue[int],
    ack: bool,
    received_counts: list[int],
    progress_every: int,
) -> None:
    async with websockets.connect(
        f"{url}?format={wire_format}",
        max_size=None,
        ping_interval=None,
        ping_timeout=None,
    ) as websocket:
        await send_message(websocket, {"action": "subscribe", "topic": topic}, wire_format)
        while True:
            message = decode_message(await websocket.recv(), wire_format)
            if message.get("action") == "subscribed":
                await ready.put(subscriber_id)
                break
            if message.get("action") == "deliver" and ack:
                await send_message(websocket, {"action": "ack", "message_id": message["message_id"]}, wire_format)

        received = 0
        while received < target_messages:
            raw = await websocket.recv()
            message = decode_message(raw, wire_format)
            if message.get("action") != "deliver":
                continue
            received += 1
            if received % progress_every == 0 or received == target_messages:
                received_counts[subscriber_id] = received
            if ack:
                await send_message(websocket, {"action": "ack", "message_id": message["message_id"]}, wire_format)
        received_counts[subscriber_id] = received
        await counter.put(received)


async def publisher(url: str, topic: str, wire_format: WireFormat, count: int, publisher_id: int) -> None:
    async with websockets.connect(
        f"{url}?format={wire_format}",
        max_size=None,
        ping_interval=None,
        ping_timeout=None,
    ) as websocket:
        for index in range(count):
            await send_message(
                websocket,
                {
                    "action": "publish",
                    "topic": topic,
                    "payload": {
                        "publisher_id": publisher_id,
                        "sequence": index,
                        "temperature": 22.5,
                    },
                },
                wire_format,
            )
        await send_message(websocket, {"action": "flush"}, wire_format)
        while True:
            message = decode_message(await websocket.recv(), wire_format)
            if message.get("action") == "flushed":
                break


async def run_benchmark(args: argparse.Namespace) -> None:
    if args.topic is None:
        args.topic = f"benchmark-{args.format}-{datetime.now().strftime('%Y%m%d%H%M%S')}"

    total_published = args.publishers * args.messages
    expected_per_subscriber = total_published
    counter: asyncio.Queue[int] = asyncio.Queue()
    ready: asyncio.Queue[int] = asyncio.Queue()
    received_counts = [0 for _ in range(args.subscribers)]
    delivered = 0
    progress_task: asyncio.Task | None = None

    subscribers = [
        asyncio.create_task(
            subscriber(
                subscriber_id,
                args.url,
                args.topic,
                args.format,
                expected_per_subscriber,
                counter,
                ready,
                args.ack,
                received_counts,
                args.progress_every,
            )
        )
        for subscriber_id in range(args.subscribers)
    ]
    for _ in subscribers:
        await asyncio.wait_for(ready.get(), timeout=args.timeout)
    await asyncio.sleep(args.subscriber_warmup)

    start = time.perf_counter()

    async def print_progress() -> None:
        while True:
            await asyncio.sleep(args.progress_interval)
            elapsed = time.perf_counter() - start
            total_received = sum(received_counts)
            min_received = min(received_counts) if received_counts else 0
            max_received = max(received_counts) if received_counts else 0
            print(
                f"Still running: {counter.qsize()}/{args.subscribers} subscribers finished "
                f"after {elapsed:.1f} s; received total={total_received}, "
                f"min/sub={min_received}, max/sub={max_received}, "
                f"target/sub={expected_per_subscriber}",
                flush=True,
            )

    if args.progress_interval > 0:
        progress_task = asyncio.create_task(print_progress())

    try:
        await asyncio.gather(
            *[
                publisher(args.url, args.topic, args.format, args.messages, publisher_id)
                for publisher_id in range(args.publishers)
            ]
        )

        for _ in subscribers:
            delivered += await asyncio.wait_for(counter.get(), timeout=args.timeout)

    except (asyncio.TimeoutError, ConnectionClosed) as exc:
        print(f"Benchmark failed before all subscribers finished: {exc}")
        raise

    finally:
        if progress_task is not None:
            progress_task.cancel()
            with contextlib.suppress(asyncio.CancelledError):
                await progress_task

        for task in subscribers:
            task.cancel()
        await asyncio.gather(*subscribers, return_exceptions=True)

    elapsed = time.perf_counter() - start

    print(f"Format: {args.format}")
    print(f"Topic: {args.topic}")
    print(f"ACK enabled: {args.ack}")
    print(f"Publishers: {args.publishers}")
    print(f"Subscribers: {args.subscribers}")
    print(f"Published messages: {total_published}")
    print(f"Delivered messages: {delivered}")
    print(f"Elapsed: {elapsed:.3f} s")
    print(f"Throughput: {delivered / elapsed:.2f} msg/s")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Benchmark the WebSocket message broker")
    parser.add_argument("--url", default="ws://localhost:8000/broker")
    parser.add_argument("--topic", default=None, help="Topic name. Defaults to a fresh topic per run.")
    parser.add_argument("--format", choices=["json", "msgpack"], default="json")
    parser.add_argument("--publishers", type=int, default=5)
    parser.add_argument("--subscribers", type=int, default=5)
    parser.add_argument("--messages", type=int, default=1_000)
    parser.add_argument("--subscriber-warmup", type=float, default=0.2)
    parser.add_argument("--timeout", type=float, default=300.0)
    parser.add_argument("--progress-interval", type=float, default=5.0)
    parser.add_argument("--progress-every", type=int, default=100)
    parser.add_argument("--ack", action="store_true", help="ACK every delivered message. Slower, measures durable ACK overhead.")
    return parser


def main() -> None:
    asyncio.run(run_benchmark(build_parser().parse_args()))


if __name__ == "__main__":
    main()
