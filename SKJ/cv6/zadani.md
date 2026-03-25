🎯 Cíl

Cílem tohoto cvičení je navrhnout a implementovat základní verzi object storage služby, inspirované cloudovými úložišti (např. Amazon S3).

Výsledkem bude backendová služba, která umožní:

    nahrávání souborů
    jejich ukládání na disk
    správu metadat
    stažení a smazání souborů

Tato služba bude ve výsledku součástí většího projektu (mini cloud platformy) a bude sloužit jako úložiště vstupních a výstupních dat pro další komponenty.

Při realizaci se předpokládá využití AI nastrojů dle Vašeho výběru.

Jedná se o týmovou úlohu. Na cvičení utvořte týmy o velikosti 1 - 3 členů. Ideálně si sesedněte a řešte úlohu společně.
🧱 Kontext projektu

Implementovaná služba bude využita jako:

    obecné cloudové úložiště pro uživatele
    úložiště vstupů pro výpočetní úlohy (jobs)
    úložiště výstupů

🧩 Zadání (část realizovaná na cvičení)
1. Návrh API

Navrhněte REST API pro práci se soubory:

POST /files/upload
GET /files/{id}
DELETE /files/{id}
GET /files

2. Upload souboru

Implementujte endpoint:

POST /files/upload

Požadavky:

    přijímá soubor pomocí multipart/form-data
    uloží soubor na disk
    vygeneruje unikátní identifikátor souboru
    vrátí metadata ve formátu:

{
  "id": "string",
  "filename": "string",
  "size": 1234
}

Uložení informací o souborech zatím můžete realizovat jako jednoduchý JSON soubor. Pokud chcete, můžete použít rovnou i SQL databázi za použití knihovny SQLAlchemy.
3. Ukládání souborů

Navrhněte strukturu ukládání na disku, například:

storage/
   <user_id>/
       <file_id>

Požadavky:

    soubory různých uživatelů musí být oddělené
    nesmí docházet ke kolizím názvů

4. Metadata

Metadata ukládejte do databáze.

Minimální struktura:

File:
- id
- user_id
- filename
- path
- size
- created_at

5. Stažení souboru

Implementujte endpoint:

GET /files/{id}

Požadavky: - vrací obsah souboru - ověřuje, že uživatel má k souboru přístup
6. Smazání souboru

Implementujte:

DELETE /files/{id}

Požadavky: - smaže soubor ze storage - odstraní metadata z databáze
🧰 Použité technologie

Použijte následující technologie, ale můžete použít i další:

    FastAPI
    SQLAlchemy
    python-multipart
    aiofiles

🏁 Minimální výstup ze cvičení

    funkční API server bez SQLAlchemy
    upload souboru
    uložení na disk
    uložení metadat
    možnost stažení souboru

🏠 Domácí úloha (rozšíření) - Bude součástí projektu

Domácí úloha bude upřesněna později.
🧠 AI část (povinná)

Součástí odevzdání bude krátký report ve formátu Markdown, který bude obsahovat:

    jaké nástroje AI byly použity
    příklady promptů
    co AI vygenerovala správně
    co bylo nutné opravit
    jaké chyby AI udělala

🧪 Testování

Doporučení:

    použijte curl nebo Postman

Příklady:

    pro POST:

curl -F "file=@test.txt" http://localhost:8000/files/upload

    pro DELETE

curl -X DELETE http://localhost:8000/files/123
