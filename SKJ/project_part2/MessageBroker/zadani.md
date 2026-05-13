V moderním cloudu spolu mikroslužby často nekomunikují napřímo (synchronně přes HTTP nebo sockety), ale asynchronně pomocí Message Brokerů (jako je RabbitMQ, Apache Kafka, AWS SNS (Simple Notification Service), AWS SQS (Simple Queue Service)). To umožňuje obrovskou škálovatelnost a odolnost proti výpadkům.

Vaším úkolem v tomto cvičení je vytvořit vlastní zjednodušený Message Broker využívající návrhový vzor Publish/Subscribe.
Úvod do asynchronního programování v Pythonu

Dosud jste psali kód synchronně. To znamená, že Python četl váš kód řádek po řádku, a pokud narazil na nějakou pomalou operaci (například stahování souboru z internetu nebo dotaz do databáze), celý program se zastavil a čekal, dokud operace neskončila. Teprve pak pokračoval na další řádek.

Při tvorbě webových serverů a cloudových aplikací je toto čekání obrovský problém. Pokud váš server obsluhuje 1000 uživatelů a u prvního z nich čeká 2 vteřiny na odpověď z databáze, zbylých 999 uživatelů by muselo čekat ve frontě.

Řešením je asynchronní programování.
Analogie: Kuchař v restauraci (aneb vysvětlení ve stylu přednášky)

Představte si, že jste kuchař a máte uvařit těstoviny a nakrájet zeleninu.

    Synchronní kuchař: Dáte vařit vodu. Stojíte u sporáku a 10 minut se upřeně díváte na hrnec, dokud voda nezačne vřít. Až pak jdete krájet zeleninu. Ztratili jste 10 minut času.

    Asynchronní kuchař: Dáte vařit vodu. Nastavíte si budík a řeknete si: "Tohle bude chvíli trvat, jdu zatím dělat něco jiného". Jdete krájet zeleninu. Když budík zazvoní (voda vře), přerušíte krájení, vhodíte těstoviny, a zase se vrátíte k zelenině.

Asynchronní Python funguje přesně jako ten druhý kuchař. Nepřidává vám do kuchyně další kuchaře (to by byl multithreading/multiprocessing), ale učí toho jednoho kuchaře nečekat na pomalé věci a využít čas efektivně.
Klíčová slova: async a await

V Pythonu (od verze 3.5) se asynchronní kód píše pomocí dvou klíčových slov:

    async def: Tímto definujete asynchronní funkci (tzv. korutinu). Říkáte tím Pythonu: "Pozor, uvnitř této funkce se možná bude na něco čekat."

    await: Toto je ten moment, kdy kuchař odchází od hrnce. Napíšete ho před volání pomalé I/O operace. Říkáte tím: "Pythone, tady teď budeme čekat na síť/disk. Běž zatím dělat jinou práci, a až to bude hotové, vrať se sem."

Příklad 1: Synchronní vs. asynchronní kód

Zkuste si porovnat tyto dva zápisy. Použijeme funkci sleep(), která simuluje pomalou operaci (např. čekání na síť).

Špatně (Synchronně):

import time

def uvar_testoviny():
    print("Dávám vařit vodu...")
    time.sleep(3)  # Program se zde na 3 vteřiny totálně zasekne
    print("Těstoviny jsou hotové!")

def nakrajej_zeleninu():
    print("Krájím zeleninu...")
    time.sleep(2)  # Zaseknutí na 2 vteřiny
    print("Zelenina nakrájena!")

def main():
    start = time.time()
    uvar_testoviny()
    nakrajej_zeleninu()
    print(f"Celkový čas: {time.time() - start:.2f} sekund") # Trvá to 5 sekund (3+2)

main()

Správně (asynchronně pomocí modulu asyncio):

import asyncio
import time

# Přidali jsme 'async'
async def uvar_testoviny():
    print("Dávám vařit vodu...")
    # 'await' pustí Event Loop (manažera), aby zatím dělal něco jiného
    await asyncio.sleep(3)
    print("Těstoviny jsou hotové!")

async def nakrajej_zeleninu():
    print("Krájím zeleninu...")
    await asyncio.sleep(2)
    print("Zelenina nakrájena!")

async def main():
    start = time.time()
    # asyncio.gather spustí více asynchronních funkcí "současně"
    await asyncio.gather(
        uvar_testoviny(),
        nakrajej_zeleninu()
    )
    print(f"Celkový čas: {time.time() - start:.2f} sekund") # Trvá to jen 3 sekundy!

# Asynchronní program musíme spustit přes Event Loop
asyncio.run(main())

Všimněte si, že asynchronní verze trvala jen 3 sekundy (čas té nejdelší operace), protože kuchař krájel zeleninu během toho, co se vařila voda.
Jak to souvisí s FastAPI a vaším Brokerem?

FastAPI je asynchronní framework. Když vytvoříte endpoint takto:

@app.get("/data")
async def get_data():
    data = await database.fetch_data()
    return data

FastAPI si spustí "Event Loop" (nekonečnou smyčku, která funguje jako manažer úloh).

    Připojí se Klient A a chce data.

    Server narazí na await database.fetch_data().

    Místo toho, aby server zamrznul a čekal na pomalou databázi, Event Loop funkci fetch_data "uspí".

    Připojí se Klient B. Server ho okamžitě obslouží, protože má volné ruce.

    Jakmile databáze pošle data pro Klienta A, Event Loop probudí jeho funkci a odešle mu výsledek.

Při programování WebSocketů (await websocket.receive_text()) je toto naprosto nezbytné (k čemu WebSockety si řekneme níže). Pokud byste nepoužili await, první klient, který by se připojil a neposílal žádné zprávy, by natrvalo zablokoval celý váš server a nikdo další by se nepřipojil.
Jak funguje Publish/Subscribe (Pub/Sub)

V následujících řádcích si popíšeme, jak funguje Pub/Sub.
Klasická datová struktura fronta

Zajisté znáte datovou strukturu fronta (v Pythonu implementována jako Queue). Tato struktura může být využita pro posílání zpráv. Je to však komunikační kanál, který je použitelný v rámci jednoho programu, neboť sdílí pamět daného procesu (nebavíme se nyní o IPC frontě v operačním systému). Pokud daný proces spadne, veškerá data ve frontě jsou ztracena. Naproti tomu message broker je samostatný proces s perzistentním uložištěm, prostřednictvím kterého může spolu komunikovat více procesů (programů).
Pub/Sub model a motivační příklad

V Pub/Sub modelu spolu komponenty nekomunikují přímo, ale přes prostředníka (message broker). Zkusme se nejdříve podívat na motivační příklad.

Představte si, že stavíte backend pro velký e-shop. Zákazník klikne na tlačítko "Dokončit objednávku". Co všechno se musí stát?

    Platba: Musí se strhnout peníze z karty.
    Sklad: Musí se odečíst zboží z inventáře.
    E-mail: Zákazníkovi musí přijít potvrzení s PDF fakturou.
    Doprava: Musí se vygenerovat štítek pro kurýra.
    Věrnostní systém: Zákazníkovi se musí přičíst body.

Řešní 1: Synchronní "špagetový [EN] kód" (jen to ne!)

Pokud byste psali kód synchronně a bez návrhového vzoru Pub/Sub, vypadalo by to takto: Webový server přijme požadavek a postupně (nebo i paralelně, ale napřímo) volá API všech ostatních služeb. Takový webový server je pak provázán ze všem ostatními službami.
Problémy

    Pomalost: Zákazník čeká, než se vygeneruje PDF, než odpoví sklad a než se spojíme s dopravcem. Zpracování trvá třeba 5 vteřin. Zákazník si myslí, že to zamrzlo, a klikne na tlačítko znovu.

    Křehkost (Tight Coupling): Co když má služba pro odesílání e-mailů výpadek? Celá objednávka spadne a zákazník si nic nekoupí jen proto, že nešlo odeslat e-mail. Služby o sobě musí vědět (webový server musí znát IP adresy skladu, e-mailu, dopravy).

    Nebezpečné úpravy: Zítra přijde marketingové oddělení, že chtějí poslat SMS upozornění. Musíte zasáhnout do kritického kódu pro vytváření objednávek a přidat tam další API volání.

Řešení 2: Publish/Subscribe (mnohem lepší)

Místo toho, aby webový server obvolával pět různých služeb, udělá jedinou věc: "vyšle" do světa zprávu.

Webový server vytvoří zprávu ve formátu JSON (může samozřejmě použít i jiný formát): {"event": "ORDER_CREATED", "order_id": 12345} a odešle ji na jedno jediné místo. Tím to pro něj končí, okamžitě vrací zákazníkovi odpověď na webu: "Děkujeme, objednávka se zpracovává!" (Celé to trvalo 150 milisekund).

Jak funguje vzor Pub/Sub v tomto scénáři:

    Publisher (Vydavatel): Náš webový server. Vydává zprávu o tom, že se něco stalo (událost). Vůbec ho nezajímá, kdo zprávu čte.

    Subscriber (Odběratel): Ostatní služby (sklad, e-mail, doprava). Tyto služby "odebírají" téma (topic) ORDER_CREATED.

    Message broker: Zprostředkovává komunikaci - přijímá zprávy od publisherů a doručuje je subscriberům.

    Volná vazba (Decoupling): Webový server neví o e-mailové službě. E-mailová služba neví o skladu. Zítra můžeme přidat novou službu pro SMS notifikace - prostě ji jen přihlásíme k odběru tématu ORDER_CREATED. Webového serveru se to nijak nedotkne. Pokud e-mailová služba spadne, zpráva na ni počká.

Schématicky vypadá Pub/Sub následovně:

          +-------------+
          |  Publisher  |
          +-------------+
                 |
                 |  zpráva (event)
                 v
        +---------------------+
        |   Message Broker    |
        +---------------------+
           /        |        \
          /         |         \
         v          v          v
+--------------+ +--------------+ +--------------+
| Subscriber 1 | | Subscriber 2 | | Subscriber 3 |
+--------------+ +--------------+ +--------------+

Možná Vás napadne, že Pub/Sub je podobný návrhovému vzoru EventListener, který můžete znát třeba z tvorby GUI. Strukturálně jsou si oba vzory podobné a liší se především měřítkem, tedy podobě jako fronta, která žije v jednom programu i EventListener žije v jednom programu. Pub/Sub pak žije nezávisle na procesech a umožňuje jejich komunikaci.
Jaký je vztah mezi Pub/Sub a Message Brokerem?

Tyto dva pojmy se často pletou, ale rozdíl je jednoduchý:

    Publish/Subscribe je návrhový vzor, který říká, jak by spolu měly komponenty komunikovat (vydavatelé a nezávislí odběratelé).

    Message Broker je software (program), který tento vzor uvádí do praxe. Je to něco jako "pošta" nebo "nástěnka".

Vydavatel nevydává zprávu jen tak do prázdna, posílá ji Message Brokeru (např. RabbitMQ, Kafka). Message Broker zprávu přijme, podívá se, do jakého patří tématu (Topic), a rozešle ji všem odběratelům, kteří si u něj toto téma zaregistrovali. Broker řeší tu těžkou práci: síťování, výpadky, udržení zpráv v paměti nebo na disku.
Souvislost s asynchronním programováním

Asynchronní programování (v Pythonu přes async/await a asyncio) je pro tento systém absolutním základem na straně kódu.

    Nechceme blokovat vlákna (Non-blocking I/O) Když klient odesílá zprávu brokeru přes síť, nebo když naopak Subscriber čeká, až mu broker pošle nějakou zprávu, probíhá síťová komunikace (I/O operace). Kdybychom použili synchronní kód (např. obyčejné requests.get() nebo synchronní sockety), procesor by se v tu chvíli zablokoval a "čekal". V případě FastAPI s asynchronními WebSockety může server vyřizovat tisíce jiných spojení, zatímco čeká, než mu přijde další zpráva.

    Event Loop (Smyčka událostí) Odběratel (např. služba pro odesílání e-mailů) typicky běží v nekonečné asynchronní smyčce. Kód vypadá zhruba takto:

    while True:
        message = await broker.receive_message()

    Všimněte si použití klíčového slova await. Program neustále "poslouchá", ale neplýtvá přitom výkonem procesoru (jako by to dělal synchronní while True cyklus, který by vytížil CPU na 100 %). CPU může dělat jinou práci a k tomuto kódu se vrátí přesně v okamžiku, kdy broker přes síť pošle nová data.

    Paralelní zpracování u klienta Díky asynchronnímu programování může jeden proces v Pythonu naslouchat na více tématech najednou, nebo například v benchmarkovacím skriptu (z předchozího zadání) spustit 10 klientů současně v jednom vlákně pomocí asyncio.gather(), což je ideální pro simulaci reálné cloudové zátěže.

Úkol 1: Implementace Brokera (FastAPI WebSockets)

Vytvořte ve FastAPI WebSocketový endpoint /broker. Broker musí:

    Přijímat připojení od více klientů současně.
    Spravovat "témata" (Topics): Klient se může přihlásit k odběru konkrétního tématu (Subscribe).
    Směrovat zprávy (Routing): Pokud klient pošle zprávu (Publish) do konkrétního tématu, broker ji okamžitě přepošle všem klientům, kteří dané téma odebírají.
    Řešit odpojení: Pokud spojení spadne, broker musí klienta bezpečně odebrat ze seznamu odběratelů, aby aplikace nespadla.

    Nápověda k architektuře: Vytvořte si třídu ConnectionManager, která si bude v paměti (např. ve slovníku) držet mapování: {"nazev_tematu": [websocket1, websocket2, ...]}.

Úkol 2: Klient a podpora více formátů zpráv

Napište v Pythonu (pomocí knihovny websockets (pip) a asyncio) klientský skript, např. mb_client.py. Skript by měl umět fungovat ve dvou režimech: Publisher (odesílatel) a Subscriber (příjemce).

V cloudu se platí za přenesená data a čas procesoru. Textové formáty jsou čitelné, ale binární jsou efektivnější. Váš broker a klient musí podporovat dva různé formáty serializace (přepínatelné při inicializaci):

    JSON (textový, standardní).
    MessagePack (binární, vyžaduje instalaci knihovny msgpack).

Zpráva, kterou klient odesílá brokeru, by mohla vypadat logicky nějak takto: {"action": "publish", "topic": "sensors", "payload": {"temp": 22.5}}. Ve formátu MessagePack se tato struktura zkomprimuje do binární podoby.
Úkol 3: Zátěžové testy a měření propustnosti (benchmarking)

Zjistěte, jak rychlý váš broker je. Napište skript benchmark.py, který:

    Pomocí asyncio.gather vytvoří např. 5 souběžných Subscriberů a 5 souběžných Publisherů.
    Každý Publisher odešle v rychlém sledu 10 000 zpráv.
    Skript změří celkový čas, za který všichni Subscribeři přijmou všechny zprávy.
    Vypočítá a vypíše propustnost (throughput) v metrice zprávy za sekundu (msg/s).

Proveďte měření pro formát JSON a pro MessagePack a výsledky porovnejte.

    Poznámka: Broker musíte spustit přes Uvicorn.

Úkol 4: Automatizované testy (pytest)

Napište automatizované integrační testy pro vašeho brokera. Použijte knihovny pytest, pytest-asyncio a httpx (které podporuje testování WebSocketů přes FastAPI TestClient). Otestujte minimálně tyto scénáře:

    Úspěšné připojení a odpojení klienta.
    Zpráva odeslaná do tématu X dorazí klientovi, který odebírá téma X.
    Zpráva odeslaná do tématu Y nedorazí klientovi, který odebírá pouze téma X.

Úkol 5: Garantované doručení a perzistence (Durable Queues)

V cloudu se nesmí stát, že se důležitá data ztratí jen proto, že mikroslužba zrovna procházela restartem, nebo na sekundu vypadlo síťové spojení. Váš dosavadní broker zprávy jen přeposílal - pokud odběratel (Subscriber) nebyl zrovna v dané milisekundě připojen, zpráva nenávratně zmizela (tady se už může vaše implementace lišit, ale nějak jsme tiše předpokládali, že si zprávy nikde neukládáte).

Profesionální brokery (jako AWS SQS nebo RabbitMQ) používají tzv. Durable Queues (perzistentní fronty). Zprávy si ukládají, dokud si nejsou absolutně jisti, že je klient bezpečně zpracoval.

Vaším úkolem je tuto logiku implementovat:

    Databázový model (migrace): - Pomocí SQLAlchemy vytvořte nový model QueuedMessage. - Bude obsahovat: id (Primary Key), topic (String), payload (String nebo LargeBinary pro MessagePack), created_at (DateTime) a is_delivered (Boolean, defaultně False). - Pomocí nástroje Alembic vygenerujte a aplikujte příslušnou migraci.

    Ukládání zpráv (Publisher flow): - Jakmile broker přijme od Publishera novou zprávu, nejprve ji trvale uloží do databáze (s is_delivered = False) a přidělí jí unikátní ID. Až poté ji rozešle aktivním odběratelům.

    Načtení po výpadku (Subscriber flow): - Když se klient připojí jako Subscriber k nějakému tématu (např. sensors), broker mu přednostně odešle všechny historické zprávy z databáze pro dané téma, které mají is_delivered == False.

    Potvrzení o zpracování (ACK - Acknowledgment): - Samo odeslání zprávy přes WebSocket nestačí (spojení může spadnout během přenosu). - Klient musí po přijetí a zpracování zprávy odeslat brokeru potvrzení (tzv. ACK), např. ve formátu: {"action": "ack", "message_id": 42}. - Jakmile broker obdrží ACK pro zprávu 42, označí ji v databázi jako is_delivered = True (případně ji může rovnou smazat, tzv. Soft vs. Hard delete).

    Poznámka: Pozor na blokující operace. Databázové dotazy (pokud používáte synchronní SQLAlchemy Session) mohou zablokovat asynchronní event loop WebSocketů. AI vám může poradit, jak využít run_in_threadpool z FastAPI, případně jak přejít na asynchronní SQLAlchemy (AsyncSession). Do AI reportu popište, jaké řešení jste zvolili a proč.

    Nápověda k architektuře zpráv (protokol): Aby váš systém dokázal rozlišit mezi publikováním, odebíráním a potvrzováním, budete muset zavést jednotný "protokol" (strukturu Pydantic modelu), jak zprávy mezi klientem a brokerem vypadají.

Příklad možného protokolu:

// 1. Publisher posílá zprávu na Broker
{
  "action": "publish",
  "topic": "sensors",
  "payload": {"temperature": 22.5}
}

// 2. Broker posílá zprávu Subscriberovi (přidá ID z databáze)
{
  "action": "deliver",
  "topic": "sensors",
  "message_id": 1056,
  "payload": {"temperature": 22.5}
}

// 3. Subscriber potvrzuje úspěšné přijetí (ACK)
{
  "action": "ack",
  "message_id": 1056
}

Kostra Connection Managera

Pro inspiraci, jak si držet stav připojených klientů v paměti vašeho FastAPI serveru:

from fastapi import WebSocket

class ConnectionManager:
    def __init__(self):
        # Mapa: název tématu -> množina připojených WebSocketů
        self.active_connections: dict[str, set[WebSocket]] = {}

    async def connect(self, websocket: WebSocket, topic: str):
        await websocket.accept()
        if topic not in self.active_connections:
            self.active_connections[topic] = set()
        self.active_connections[topic].add(websocket)

    def disconnect(self, websocket: WebSocket, topic: str):
        if topic in self.active_connections:
            self.active_connections[topic].discard(websocket)
            # Pokud je téma prázdné, můžeme ho smazat pro úsporu paměti
            if not self.active_connections[topic]:
                del self.active_connections[topic]

    async def broadcast(self, message: bytes, topic: str):
        # V reálném nasazení zde použijeme asynchronní rozeslání
        # všem klientům v self.active_connections[topic]
        pass

Pravidla odevzdání a AI report

    Kód musí využívat asynchronní programování (async/await). Nepsat synchronní blokující kód v event loopu!
    Součástí odevzdání bude krátký dokument (benchmark_results.md), kde uvedete:
        Konfiguraci počítače, na kterém test běžel.
        Naměřenou propustnost pro JSON vs. MessagePack. Zhodnocení, zda se binární formát vyplatí.

    AI Report: Zaměřte se na to, jak vám AI pomohlo s návrhem ConnectionManageru a psaním asynchronních testů v Pytestu (což bývá často ošemetné).
