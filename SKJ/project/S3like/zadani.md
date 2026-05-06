V minulém cvičení jsme naimplementovali perzistenci dat pomocí SQLAlchemy a validaci přes Pydantic. Naše databáze se ale bude vyvíjet. V reálném světě nemůžeme při každé změně modelů smazat celou databázi (např. todo.db) a vytvořit ji znovu – přišli bychom o všechna uživatelská data.

Řešením jsou databázové migrace. Nástroj na správu migrací se stará o postupné aplikování (a případně i vracení) změn ve struktuře databáze (přidávání tabulek, sloupců, indexů).

V ekosystému SQLAlchemy je standardem knihovna Alembic.
Co jsou databázové migrace a proč je potřebujeme?

Představte si, že máte spuštěnou aplikaci (například e-shop), ve které je již zaregistrováno tisíce uživatelů. Vaše SQLAlchemy třída reprezentující uživatele vypadá, např. takto:

class User(Base):
    __tablename__ = "users"
    id: Mapped[int] = mapped_column(primary_key=True)
    name: Mapped[str] = mapped_column(String(50))

Nyní přijde zadání, že uživatelům potřebujeme začít posílat upozornění, a proto musíme do databáze nutně přidat sloupec email. Upravíme tedy model v Pythonu a přidáme nový atribut do třídy User. Ale jak tuto změnu dostaneme do existující databáze?

Špatné řešení: Smažeme soubor database.db a necháme Base.metadata.create_all() vytvořit novou, čistou databázi s novým sloupcem.

Výsledek: Databáze má správnou strukturu, ale právě jsme nevratně smazali data všech tisíců zákazníků.

    Poznámka: Poněkud lepší by bylo si databázi zazálohovat a pak do nově vytvořené databáze opětovně nahrát data. Toto obšem vyžaduje speciální script, který data upraví ze staré databáze do nové, neboť nyní se již databázová schémata neshodují a není možné databázi "jen tak nakopírovat" do nové.

Lepší řešení: Zdrojový kód tabulky User upravíme a commitneme. Pak se na produkčním systému zastaví aplikace, v databázi se zavolá SQL příkaz ALTER TABLE users ADD COLUMN email VARCHAR(100); a aplikace se opět spustí na produkčním stroji.

Výsledek: Do tabulky se přidá nový sloupec a stará data zůstanou nedotčena.

Správný řešení: Jakmile commitneme zdrojový kód s upravenou třídou User změna se bude aplikovat na produkčním systému, kde se použije migrační nástroj, který za nás vytvoří SQL příkaz ALTER TABLE users ADD COLUMN email VARCHAR(100);. Migrační nástoj by také měl zajistit, že se jednoduše "nepřepíšeme" a databázi tím neporušíme.

Výsledek: Do tabulky se přidá nový sloupec a stará data zůstanou nedotčena. (Je stejný jako v předchozím řešení, jen jsme si řešením více jisti).

    Poznámka: Automatická migrace by se měla otestovat v rámci Continuous Integration/Contunous Delivery (CI/CD) nástroje.

A přesně tento bezpečný přechod řeší databázové migrace. Můžete si je představit jako systém pro správu verzí (něco jako Git), ale pro schéma vaší databáze.

Místo abyste psali SQL příkazy pro změnu tabulek ručně, použijeme nástroj Alembic. Ten se podívá na vaše aktuální Python modely, porovná je se současným stavem databáze a automaticky vygeneruje migrační skript – soubor s kódem, který databázi postupně a bezpečně upraví (upgraduje) na novou verzi, aniž byste přišli o drahocenná data.
Inicializace Alembicu

Alembic se inicializuje přes příkazovou řádku ve složce vašeho projektu:

$ alembic init alembic

Tím se vytvoří složka alembic a soubor alembic.ini. Abyste mohli migrace generovat automaticky na základě vašich SQLAlchemy modelů, je nutné upravit soubor alembic/env.py tak, aby Alembic viděl vaše modely (importovat Base.metadata) a správně nastavit URL k vaší databázi v alembic.ini.

Základní workflow pak vypadá takto: 1. Upravíte/vytvoříte SQLAlchemy modely v kódu. 2. Vygenerujete migrační skript: alembic revision --autogenerate -m "popis zmeny" 3. Zkontrolujete vygenerovaný skript ve složce alembic/versions/. 4. Aplikujete migraci na databázi: alembic upgrade head
Úkol 1: Zavedení Bucketů (Migrace 1)

V Amazon S3 neexistují objekty jen tak "ve vakuu", ale jsou vždy součástí tzv. Bucketů (kbelíků/složek).

    Vytvořte nový SQLAlchemy model Bucket, který bude obsahovat:
        id (Primary Key)
        name (String, unikátní název bucketu)
        created_at (DateTime, datum vytvoření)
    Upravte stávající model pro uložení objektu (souboru). Přidejte k němu relaci na Bucket. Bude obsahovat nový sloupec bucket_id (Foreign Key).
    Vygenerujte a aplikujte první migraci.
    Vytvořte Pydantic modely a FastAPI endpointy pro vytváření bucketů (POST /buckets/) a výpis objektů v konkrétním bucketu (GET /buckets/{bucket_id}/objects/). Ve své podstatě jde o listování obsahu bucketů.

Úkol 2: Účtování za přenos dat (Migrace 2)

Provozovatelé cloudových úložišť si účtují poplatky nejen za uložené místo, ale i za přenesená data (bandwidth).

    Rozšiřte stávající model Bucket o nový sloupec bandwidth_bytes (Integer, defaultně 0).
    Vygenerujte a aplikujte druhou migraci. (Všimněte si, že se tabulka upraví, ale existující buckety a objekty z předchozího úkolu v DB zůstanou!)
    Upravte vaše existující endpointy pro nahrávání (upload) a stahování (download/read) metadat objektů. Každý úspěšný request musí updatovat hodnotu bandwidth_bytes u příslušného bucketu (přičíst velikost objektu k celkovému účtu).
    Vytvořte endpoint GET /buckets/{bucket_id}/billing/, který pomocí Pydantic modelu vrátí aktuální stav účtu za data pro daný bucket.

Úkol 2: Pokročilé účtování za přenos dat(Advanced Billing)

Provozovatelé cloudových úložišť si účtují poplatky nejen za uložené místo, ale i za přenesená data (bandwidth). V cloudu není "data transfer" jedna hodnota. Musíme rozlišovat, odkud a kam data tečou.

Pravidla pro účtování za přenos dat: 1. Storage (Data at rest): Platí se za průměrné množství uložených dat (v GB). 2. Ingress (Příchozí): Data tekoucí z internetu k nám. Většinou bývají zdarma, ale my je budeme pro účely cvičení logovat. 3. Egress (Odchozí): Data tekoucí od nás do internetu. Toto je nejdražší položka. 4. Internal (V rámci cloudu): Pokud data tečou mezi našimi službami (např. ze S3 do EC2 v rámci stejného regionu), jsou zdarma.
Technická realizace (Migrace 2)

    Model Bucket: Rozšiřte o sloupce: * current_storage_bytes: Aktuální součet velikostí všech (nesmazaných) objektů. * ingress_bytes: Kumulativní součet všech příchozích přenosů. * egress_bytes: Kumulativní součet všech odchozích přenosů do internetu. * internal_transfer_bytes: Kumulativní součet přenosů v rámci cloudu (pro statistiku).

    Logika "Internal vs External": V reálném světě se to pozná podle IP adresy. Pro naše účely budeme simulovat interní provoz pomocí HTTP Headeru. * Pokud request obsahuje hlavičku X-Internal-Source: true, započítá se přenos do internal_transfer_bytes. * Pokud hlavička chybí, jde o externí provoz (Ingress/Egress).

    Update při operacích: * POST /upload: Zvýší storage_bytes a (pokud není interní) zvýší ingress_bytes. * GET /download: Pokud není interní, zvýší egress_bytes. * DELETE /object: Sníží storage_bytes (pokud je Soft Delete aktivní, storage se stále počítá, dokud není objekt smazán definitivně).

Úkol 3: Ochrana proti nechtěnému smazání - Soft Delete

V cloudu znamená smazání často jen přesun do "koše" nebo zneviditelnění objektu, aby bylo možné ho po určitou dobu obnovit. K tomu se používá technika Soft Delete.

    Přidejte do modelu vašeho objektu/souboru sloupec is_deleted (Boolean, default bude False).
    Vygenerujte a aplikujte třetí migraci.
    Upravte endpoint pro smazání objektu (DELETE /objects/{object_id}). Místo fyzického odstranění záznamu z databáze (db.delete()) pouze nastavte is_deleted = True.
    Upravte endpointy pro výpis objektů. Musí nyní ve výchozím stavu filtrovat data a vracet pouze objekty, které nejsou smazané.

Úkol 4: Billing za API Requesty (Bonus / Rozšíření)

Cloudoví provideři neúčtují jen za přenesené bajty, ale i za samotné "volání" API. * PUT, POST, LIST: Jsou dražší (mění stav systému, vyžadují víc zápisů do DB). * GET: Jsou levnější.

    Migrace: Přidejte sloupce count_write_requests a count_read_requests.
    Middleware/Decorator: Implementujte logiku, která při každém volání příslušného endpointu inkrementuje dané počítadlo v databázi.

Nápověda: Jak správně propojit Alembic s vaším kódem

Aby mohl Alembic automaticky generovat migrace (--autogenerate), musí znát dvě věci: 1. Jak vypadají vaše SQLAlchemy modely (tzv. metadata). 2. Kde leží vaše databáze.

Po spuštění alembic init alembic je nutno provést dvě klíčové úpravy:
1. Úprava alembic.ini

Najděte řádek začínající sqlalchemy.url a nastavte správnou cestu k vaší SQLite databázi.

# Příklad v alembic.ini
sqlalchemy.url = sqlite:///./todo.db

    Poznámka: Pozor na relativní cesty. ./todo.db znamená, že soubor databáze leží ve složce, odkud spouštíte příkaz alembic.

2. Úprava alembic/env.py

Otevřete vygenerovaný soubor env.py a naimportujte váš Base model z FastAPI aplikace. Aby Python správně našel vaše soubory, je někdy nutné přidat kořenovou složku projektu do sys.path.

import sys
import os
from logging.config import fileConfig
from sqlalchemy import engine_from_config
from sqlalchemy import pool
from alembic import context

# 1. Přidání aktuální složky do cesty, aby šly importovat vaše modely
sys.path.append(os.path.dirname(os.path.dirname(__file__)))

# 2. Import vaší Base třídy (změňte 'models' podle názvu vašeho souboru)
from models import Base

# tento kód je generovaný Alembicem...
config = context.config
if config.config_file_name is not None:
    fileConfig(config.config_file_name)

# 3. Nastavení target_metadata na metadata vašich modelů
target_metadata = Base.metadata

# ... zbytek souboru zůstává stejný (funkce run_migrations_offline a run_migrations_online)

Častá chyba: Pokud vám Alembic hlásí ImportError, zkontrolujte, zda importujete Base ze správného souboru a zda máte správně vyřešený sys.path. Pokud vám vygeneruje prázdnou migraci (bez přidání tabulek), s největší pravděpodobností není nastaveno target_metadata = Base.metadata nebo jste nenaimportovali soubor, kde jsou vaše modely fyzicky definované.
Poznámky k řešení

    Všechny vstupy a výstupy musí být nadále striktně validovány přes Pydantic modely.
    Součástí řešení musí být složka alembic/versions obsahující všechny tři postupně vygenerované migrační skripty.
    Přiložte aktualizovaný AI report (co vám AI poradilo ohledně nastavení Alembicu a jaké případné chyby při generování relací udělalo).
