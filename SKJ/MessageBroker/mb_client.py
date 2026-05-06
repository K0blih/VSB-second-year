from __future__ import annotations

import argparse
import asyncio
import json
from typing import Any

import websockets

from protocol import WireFormat, decode_message, encode_message


def parse_payload(raw: str) -> Any:
    try:
        return json.loads(raw)
    except json.JSONDecodeError:
        return raw


async def send_message(websocket: websockets.WebSocketClientProtocol, message: dict, wire_format: WireFormat) -> None:
    encoded = encode_message(message, wire_format)
    await websocket.send(encoded)


async def receive_message(websocket: websockets.WebSocketClientProtocol, wire_format: WireFormat) -> dict:
    raw = await websocket.recv()
    return decode_message(raw, wire_format)


async def publisher(args: argparse.Namespace) -> None:
    url = f"{args.url}?format={args.format}"
    async with websockets.connect(url) as websocket:
        payload = parse_payload(args.payload)
        for index in range(args.count):
            message = {
                "action": "publish",
                "topic": args.topic,
                "payload": payload if args.count == 1 else {"index": index, "value": payload},
            }
            await send_message(websocket, message, args.format)


async def subscriber(args: argparse.Namespace) -> None:
    url = f"{args.url}?format={args.format}"
    async with websockets.connect(url) as websocket:
        await send_message(websocket, {"action": "subscribe", "topic": args.topic}, args.format)
        received = 0
        while args.limit == 0 or received < args.limit:
            message = await receive_message(websocket, args.format)
            if message.get("action") != "deliver":
                print(message)
                continue

            print(json.dumps(message, ensure_ascii=False))
            received += 1
            await send_message(websocket, {"action": "ack", "message_id": message["message_id"]}, args.format)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Message broker publisher/subscriber client")
    parser.add_argument("mode", choices=["publisher", "subscriber"])
    parser.add_argument("--url", default="ws://localhost:8000/broker")
    parser.add_argument("--topic", default="sensors")
    parser.add_argument("--format", choices=["json", "msgpack"], default="json")
    parser.add_argument("--payload", default='{"temperature": 22.5}')
    parser.add_argument("--count", type=int, default=1)
    parser.add_argument("--limit", type=int, default=0, help="Subscriber message limit. 0 means forever.")
    return parser


async def main() -> None:
    args = build_parser().parse_args()
    if args.mode == "publisher":
        await publisher(args)
    else:
        await subscriber(args)


if __name__ == "__main__":
    asyncio.run(main())
