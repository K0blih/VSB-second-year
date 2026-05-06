from __future__ import annotations

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse, RedirectResponse

from connection_manager import ConnectionManager
from database import SessionLocal, init_db
from protocol import decode_message, normalize_format, validate_message
from repository import delivery_payload, mark_delivered, save_message, undelivered_messages


app = FastAPI(title="Async Pub/Sub Message Broker")
manager = ConnectionManager()


@app.get("/")
async def root() -> RedirectResponse:
    return RedirectResponse(url="/broker")


@app.get("/broker", response_class=HTMLResponse)
async def broker_info() -> str:
    return """
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Async Pub/Sub Message Broker</title>
  <style>
    :root {
      color-scheme: light;
      --bg: #f7f8fa;
      --panel: #ffffff;
      --ink: #1d2430;
      --muted: #667085;
      --border: #d9dee7;
      --accent: #0f766e;
      --accent-dark: #115e59;
      --danger: #b42318;
      --code: #101828;
    }

    * { box-sizing: border-box; }

    body {
      margin: 0;
      min-height: 100vh;
      background: var(--bg);
      color: var(--ink);
      font-family: system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
      line-height: 1.5;
    }

    header {
      border-bottom: 1px solid var(--border);
      background: var(--panel);
    }

    .wrap {
      width: min(1120px, calc(100vw - 32px));
      margin: 0 auto;
    }

    .topbar {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 16px;
      padding: 18px 0;
    }

    h1 {
      margin: 0;
      font-size: clamp(1.35rem, 1rem + 1.2vw, 2rem);
      letter-spacing: 0;
    }

    main {
      padding: 24px 0 36px;
    }

    .status {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      min-width: 132px;
      justify-content: center;
      border: 1px solid var(--border);
      background: #f9fafb;
      border-radius: 999px;
      padding: 8px 12px;
      color: var(--muted);
      font-size: 0.92rem;
      white-space: nowrap;
    }

    .dot {
      width: 9px;
      height: 9px;
      border-radius: 999px;
      background: #98a2b3;
    }

    .status.connected {
      color: #027a48;
      border-color: #abefc6;
      background: #ecfdf3;
    }

    .status.connected .dot { background: #12b76a; }
    .status.error {
      color: var(--danger);
      border-color: #fecdca;
      background: #fef3f2;
    }
    .status.error .dot { background: var(--danger); }

    .grid {
      display: grid;
      grid-template-columns: minmax(300px, 390px) minmax(0, 1fr);
      gap: 20px;
      align-items: start;
    }

    .panel {
      background: var(--panel);
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 18px;
    }

    .stack {
      display: grid;
      gap: 16px;
    }

    .section-title {
      margin: 0 0 12px;
      font-size: 1rem;
      font-weight: 700;
    }

    label {
      display: grid;
      gap: 6px;
      color: var(--muted);
      font-size: 0.9rem;
      font-weight: 600;
    }

    input, textarea {
      width: 100%;
      border: 1px solid var(--border);
      border-radius: 6px;
      background: #fff;
      color: var(--ink);
      font: inherit;
      padding: 10px 11px;
    }

    textarea {
      min-height: 122px;
      resize: vertical;
      font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace;
      font-size: 0.9rem;
      color: var(--code);
    }

    input:focus, textarea:focus {
      outline: 2px solid rgba(15, 118, 110, 0.18);
      border-color: var(--accent);
    }

    .row {
      display: flex;
      gap: 10px;
      flex-wrap: wrap;
      align-items: center;
    }

    .row > * {
      flex: 1 1 150px;
    }

    .checkbox {
      display: flex;
      align-items: center;
      gap: 8px;
      color: var(--ink);
      font-weight: 500;
    }

    .checkbox input {
      width: 16px;
      height: 16px;
    }

    button {
      border: 1px solid var(--accent);
      border-radius: 6px;
      background: var(--accent);
      color: #fff;
      font: inherit;
      font-weight: 700;
      min-height: 42px;
      padding: 9px 13px;
      cursor: pointer;
    }

    button:hover { background: var(--accent-dark); }
    button:disabled {
      cursor: not-allowed;
      opacity: 0.52;
    }

    button.secondary {
      background: #fff;
      color: var(--accent);
    }

    button.secondary:hover {
      background: #f0fdfa;
    }

    button.danger {
      border-color: #fecdca;
      background: #fff;
      color: var(--danger);
    }

    button.danger:hover {
      background: #fef3f2;
    }

    .log {
      min-height: 180px;
      max-height: 300px;
      overflow: auto;
      background: #111827;
      color: #d1d5db;
      border-radius: 8px;
      padding: 14px;
      font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace;
      font-size: 0.86rem;
    }

    .event {
      padding: 10px 0;
      border-bottom: 1px solid rgba(255,255,255,0.09);
      white-space: pre-wrap;
      overflow-wrap: anywhere;
    }

    .event:last-child { border-bottom: 0; }
    .event .meta {
      color: #93c5fd;
      display: block;
      margin-bottom: 3px;
    }

    .event.error .meta { color: #fca5a5; }
    .event.in .meta { color: #86efac; }
    .event.out .meta { color: #fcd34d; }

    .hint {
      margin: 8px 0 0;
      color: var(--muted);
      font-size: 0.9rem;
    }

    .subscribers {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
      gap: 16px;
    }

    .subscriber-card {
      display: grid;
      gap: 14px;
    }

    .subscriber-head {
      display: flex;
      align-items: flex-start;
      justify-content: space-between;
      gap: 12px;
    }

    .subscriber-title {
      margin: 0;
      font-size: 1rem;
    }

    .subscriber-topic {
      margin: 2px 0 0;
      color: var(--muted);
      font-size: 0.88rem;
      overflow-wrap: anywhere;
    }

    .inbox {
      display: grid;
      gap: 8px;
      min-height: 120px;
      max-height: 240px;
      overflow: auto;
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 10px;
      background: #f9fafb;
    }

    .empty {
      color: var(--muted);
      font-size: 0.9rem;
    }

    .message {
      border: 1px solid #c7d7fe;
      border-radius: 6px;
      background: #eef4ff;
      padding: 9px;
      font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace;
      font-size: 0.84rem;
      white-space: pre-wrap;
      overflow-wrap: anywhere;
    }

    @media (max-width: 820px) {
      .grid { grid-template-columns: 1fr; }
      .topbar { align-items: flex-start; flex-direction: column; }
      .status { justify-content: flex-start; }
      .log { min-height: 360px; max-height: none; }
    }
  </style>
</head>
<body>
  <header>
    <div class="wrap topbar">
      <h1>Async Pub/Sub Message Broker</h1>
      <div id="status" class="status connected"><span class="dot"></span><span>Testing UI ready</span></div>
    </div>
  </header>

  <main class="wrap grid">
    <section class="stack">
      <div class="panel">
        <h2 class="section-title">Broker</h2>
        <label>
          WebSocket URL
          <input id="url" autocomplete="off">
        </label>
        <p class="hint">Each subscriber card opens its own JSON WebSocket connection.</p>
      </div>

      <div class="panel">
        <h2 class="section-title">New Subscriber</h2>
        <div class="stack">
          <label>
            Name
            <input id="subscriberName" value="Subscriber 1" autocomplete="off">
          </label>
          <label>
            Topic
            <input id="subscriberTopic" value="sensors" autocomplete="off">
          </label>
          <label class="checkbox">
            <input id="subscriberAutoAck" type="checkbox" checked>
            Ack delivered messages automatically
          </label>
          <button id="addSubscriber">Add subscriber</button>
        </div>
      </div>

      <div class="panel">
        <h2 class="section-title">Publish</h2>
        <div class="stack">
          <label>
            Topic
            <input id="publishTopic" value="sensors" autocomplete="off">
          </label>
          <label>
            Payload
            <textarea id="payload">{"temperature": 22.5}</textarea>
          </label>
          <button id="publish">Publish</button>
        </div>
      </div>
    </section>

    <section class="stack">
      <div class="row" style="justify-content: space-between;">
        <h2 class="section-title" style="margin: 0;">Subscribers</h2>
        <button id="clearSubscribers" class="secondary" style="flex: 0 0 auto; min-height: 36px;">Remove all</button>
      </div>
      <div id="subscribers" class="subscribers" aria-live="polite"></div>
    </section>
  </main>

  <script>
    const els = {
      addSubscriber: document.querySelector("#addSubscriber"),
      clearSubscribers: document.querySelector("#clearSubscribers"),
      payload: document.querySelector("#payload"),
      publish: document.querySelector("#publish"),
      publishTopic: document.querySelector("#publishTopic"),
      status: document.querySelector("#status"),
      subscriberAutoAck: document.querySelector("#subscriberAutoAck"),
      subscriberName: document.querySelector("#subscriberName"),
      subscribers: document.querySelector("#subscribers"),
      subscriberTopic: document.querySelector("#subscriberTopic"),
      url: document.querySelector("#url"),
    };

    const subscribers = new Map();
    let nextSubscriberId = 1;

    function defaultUrl() {
      const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
      return `${protocol}//${window.location.host}/broker?format=json`;
    }

    function timestamp() {
      return new Date().toLocaleTimeString();
    }

    function log(target, kind, title, data) {
      const entry = document.createElement("div");
      entry.className = `event ${kind}`;
      const body = typeof data === "string" ? data : JSON.stringify(data, null, 2);
      entry.innerHTML = `<span class="meta">${timestamp()} ${title}</span>${escapeHtml(body)}`;
      target.prepend(entry);
    }

    function escapeHtml(value) {
      return value
        .replaceAll("&", "&amp;")
        .replaceAll("<", "&lt;")
        .replaceAll(">", "&gt;")
        .replaceAll('"', "&quot;")
        .replaceAll("'", "&#039;");
    }

    function parseJsonField(field, fallbackToString = false) {
      const value = field.value.trim();
      if (!value) {
        return null;
      }
      try {
        return JSON.parse(value);
      } catch (error) {
        if (fallbackToString) {
          return value;
        }
        throw error;
      }
    }

    els.url.value = defaultUrl();

    function createSubscriberCard(name, topic, autoAck) {
      const id = nextSubscriberId++;
      const card = document.createElement("article");
      card.className = "panel subscriber-card";
      card.innerHTML = `
        <div class="subscriber-head">
          <div>
            <h3 class="subscriber-title">${escapeHtml(name)}</h3>
            <p class="subscriber-topic">topic: ${escapeHtml(topic)}</p>
          </div>
          <div class="status"><span class="dot"></span><span>Connecting</span></div>
        </div>
        <div>
          <h4 class="section-title">Inbox</h4>
          <div class="inbox"><div class="empty">No delivered messages yet.</div></div>
        </div>
        <div>
          <h4 class="section-title">Subscriber log</h4>
          <div class="log"></div>
        </div>
        <div class="row">
          <button class="secondary flush">Flush</button>
          <button class="danger disconnect">Disconnect</button>
        </div>
      `;

      els.subscribers.prepend(card);

      const state = {
        autoAck,
        card,
        id,
        inbox: card.querySelector(".inbox"),
        log: card.querySelector(".log"),
        name,
        socket: null,
        status: card.querySelector(".status"),
        topic,
      };
      subscribers.set(id, state);

      card.querySelector(".disconnect").addEventListener("click", () => removeSubscriber(id));
      card.querySelector(".flush").addEventListener("click", () => sendFromSubscriber(id, { action: "flush" }));

      connectSubscriber(state);
    }

    function setSubscriberStatus(state, text, className = "") {
      state.status.className = `status ${className}`.trim();
      state.status.querySelector("span:last-child").textContent = text;
    }

    function connectSubscriber(state) {
      const socket = new WebSocket(els.url.value.trim() || defaultUrl());
      state.socket = socket;

      socket.addEventListener("open", () => {
        setSubscriberStatus(state, "Subscribed", "connected");
        sendFromSubscriber(state.id, { action: "subscribe", topic: state.topic });
      });

      socket.addEventListener("message", (event) => {
        let message = event.data;
        try {
          message = JSON.parse(event.data);
        } catch (error) {
          log(state.log, "error", "received non-json", event.data);
          return;
        }

        log(state.log, "in", "received", message);
        if (message.action === "deliver") {
          addInboxMessage(state, message);
          if (state.autoAck && message.message_id != null) {
            sendFromSubscriber(state.id, { action: "ack", message_id: message.message_id });
          }
        }
      });

      socket.addEventListener("close", () => {
        setSubscriberStatus(state, "Disconnected");
        log(state.log, "in", "disconnected", "WebSocket closed.");
      });

      socket.addEventListener("error", () => {
        setSubscriberStatus(state, "Error", "error");
        log(state.log, "error", "websocket error", "The WebSocket connection failed.");
      });
    }

    function sendFromSubscriber(id, message) {
      const state = subscribers.get(id);
      if (!state || !state.socket || state.socket.readyState !== WebSocket.OPEN) {
        return;
      }
      state.socket.send(JSON.stringify(message));
      log(state.log, "out", "sent", message);
    }

    function addInboxMessage(state, message) {
      const empty = state.inbox.querySelector(".empty");
      if (empty) {
        empty.remove();
      }
      const item = document.createElement("div");
      item.className = "message";
      item.textContent = `#${message.message_id} ${JSON.stringify(message.payload, null, 2)}`;
      state.inbox.prepend(item);
    }

    function removeSubscriber(id) {
      const state = subscribers.get(id);
      if (!state) {
        return;
      }
      if (state.socket) {
        state.socket.close();
      }
      state.card.remove();
      subscribers.delete(id);
    }

    els.publish.addEventListener("click", () => {
      const topic = els.publishTopic.value.trim();
      if (!topic) {
        return;
      }
      try {
        const socket = new WebSocket(els.url.value.trim() || defaultUrl());
        const message = {
          action: "publish",
          topic,
          payload: parseJsonField(els.payload, true),
        };

        socket.addEventListener("open", () => {
          socket.send(JSON.stringify(message));
          socket.close();
        });
      } catch (error) {
        alert(`Invalid payload: ${error.message}`);
      }
    });

    els.addSubscriber.addEventListener("click", () => {
      const name = els.subscriberName.value.trim() || `Subscriber ${nextSubscriberId}`;
      const topic = els.subscriberTopic.value.trim();
      if (!topic) {
        return;
      }
      createSubscriberCard(name, topic, els.subscriberAutoAck.checked);
      els.subscriberName.value = `Subscriber ${nextSubscriberId}`;
    });

    els.clearSubscribers.addEventListener("click", () => {
      for (const id of Array.from(subscribers.keys())) {
        removeSubscriber(id);
      }
    });

    createSubscriberCard("Subscriber 1", "sensors", true);
  </script>
</body>
</html>
"""


@app.get("/health")
async def health() -> dict[str, str]:
    return {"status": "ok"}


@app.on_event("startup")
async def startup() -> None:
    await init_db()


@app.websocket("/broker")
async def broker(websocket: WebSocket, format: str = "json") -> None:
    try:
        wire_format = normalize_format(format)
    except ValueError:
        await websocket.close(code=1003)
        return

    await manager.connect(websocket, wire_format)

    try:
        while True:
            raw = await websocket.receive()
            if "text" in raw and raw["text"] is not None:
                decoded = decode_message(raw["text"], wire_format)
            elif "bytes" in raw and raw["bytes"] is not None:
                decoded = decode_message(raw["bytes"], wire_format)
            else:
                continue

            message = validate_message(decoded)

            if message.action == "subscribe":
                if not message.topic:
                    await manager.send(websocket, {"action": "error", "error": "Missing topic."})
                    continue
                await manager.subscribe(websocket, message.topic)
                historical = await undelivered_messages(SessionLocal, message.topic)
                for queued in historical:
                    await manager.send(websocket, delivery_payload(queued))
                await manager.send(websocket, {"action": "subscribed", "topic": message.topic})

            elif message.action == "publish":
                if not message.topic:
                    await manager.send(websocket, {"action": "error", "error": "Missing topic."})
                    continue
                queued = await save_message(SessionLocal, message.topic, message.payload, wire_format)
                await manager.broadcast(message.topic, delivery_payload(queued))

            elif message.action == "ack":
                if message.message_id is None:
                    await manager.send(websocket, {"action": "error", "error": "Missing message_id."})
                    continue
                await mark_delivered(SessionLocal, message.message_id)

            elif message.action == "flush":
                await manager.send(websocket, {"action": "flushed"})

    except (WebSocketDisconnect, ValueError):
        pass
    finally:
        await manager.disconnect(websocket)
