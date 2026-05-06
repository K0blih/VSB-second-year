V cloudu často potřebujeme zpracovávat velké objemy dat.

Kdybysme webový server (S3 Gateway) by se složitými výpočty nad obrázky okamžitě zahltil.
Úvod do zpracování obrazu: Všechno je jen matice

Než začneme upravovat fotgrafie, musíme pochopit, co to fotgrafie pro počítač znamená a jak se reprezentuje. V základu je to obrovská matice čísel, jejíž hodnoty odpovídají jasům jednotlivým bodům (pixelům).

Knihovna NumPy (Numerical Python) je průmyslovým standardem pro rychlé výpočty nad takovýmito maticemi. (Obecně toho tato knihovna umí mnohem více). Pokud máte obrázek o velikosti 1000 x 1000 pixelů, máte matici s milionem bodů. Každý bod (pixel) se na běžném monitoru skládá ze 3 barev: Červené (Red), Zelené (Green) a Modré (Blue) - zkráceně RGB. Každá barva má intenzitu od 0 do 255 (pro 8-mi bitové obrazy per kanál).

Barevný obrázek je tedy v NumPy reprezentován jako 3D pole (3D Array) o rozměrech (výška, šířka, 3).

    pixel = img[0, 0] nám dá barvu levého horního pixelu, např. [255, 0, 0] (čistá červená).
    Černá barva je [0, 0, 0].
    Bílá barva je [255, 255, 255].

Jak načíst obrázek do NumPy?

NumPy samo o sobě nečte .jpg nebo .png soubory. K tomu použijeme knihovnu Pillow (kdysi nazývanou PIL) a data (obrázek) z ní do NumPy jednoduše "přelijeme":

from PIL import Image
import numpy as np

# 1. Načtení obrázku do paměti
img = Image.open("peppers.png")

# 2. Převod do NumPy matice (magie začíná)
img_array = np.array(img)
print(img_array.shape)  # Vypíše např. (512, 512, 3)

# ... ZDE PROVEDEME MATEMATICKÉ OPERACE ...

# 3. Převod zpět na obrázek a uložení
new_img = Image.fromarray(img_array)
new_img.save("kocka_upravena.jpg")

Úkol: Image Processing Worker (Event-Driven)

Pokud by uživatelé začali hromadně upravovat velké fotky přímo na vašem S3 Gateway, procesor by se zahltil na 100 % a server by přestal odpovídat ostatním. Proto tuto těžkou práci (CPU-bound) přesuneme na samostatného "dělníka" (Worker) a komunikaci zajistíme přes váš Message Broker.
Architektura řešení

    Klient zavolá REST API na S3 Gateway: POST /buckets/{id}/objects/{id}/process s tělem {"operation": "grayscale"}.

    S3 Gateway okamžitě vrátí odpověď: {"status": "processing_started"} (pozor! nečeká na výsledek!). Zároveň odešle zprávu do Message Brokera do tématu image.jobs.

    Image Worker (vaše nová aplikace) naslouchá na tématu image.jobs. Jakmile zprávu přijme:
        Stáhne si původní obrázek (přes interní API z S3 Gateway).
        Provede požadovanou NumPy operaci.
        Nahraje nový upravený obrázek jako novou verzi objektu (nebo přepíše původní, zde záleží na vaší implementaci S3).
        Pošle zprávu o dokončení do tématu image.done.

Zadání pro Image Worker (NumPy operace)

Vytvořte Python skript worker.py, který běží jako nekonečná asynchronní smyčka (while True), poslouchá brokera a obsahuje logiku pro následujících 5 grafických operací.

    Veškeré úpravy musí být provedeny výhradně manipulací s NumPy maticí, nepoužívejte vestavěné filtry z knihovny Pillow (kromě úvodního načtení a finálního uložení).

1. Inverze barev (Negativ) Inverze barev patří k nejjednodušším operacím vůbec. Hodnoty barev jsou od 0 do 255. Čím je barva světlejší, tím blíže je 255. Jak udělat z bílé (255) černou (0)?

    new_array = 255 - img_array Všimněte si, že neimplementujeme žádný průchod všemi pixely v obraze, ale použijeme tzv. vektorizaci.

2. Horizontální překlopení (Zrcadlo) Zde využijeme slicing v Pythonu. Potřebujeme zachovat všechny řádky (výška), zachovat všechny barvy (hloubka), ale obrátit pořadí sloupců (šířka).

    Krok (step, stějně jako u, např. inverze stringu) -1. new_array = img_array[:, ::-1, :]

3. Ořez (Crop) Ořízněte obrázek tak, abyste odstranili okraje a ponechali, např. jen středovou oblast (výřez). Parametry pro ořezání budou předány jako parametry požadavku (requestu). V případě, že ořez je mimo dimenze obrazu, je třeba vrátít zprávy obsahující chybu a její popis.

    Opět slicing. Odřízněte např. prvních a posledních 100 pixelů na výšku i šířku. new_array = img_array[100:-100, 100:-100, :]

4. Úprava jasu (Zesvětlení) Chceme obrázek zesvětlit, tedy ke každému pixelu přičíst nějakou hodnotu (např. +50). Pozor: Datový typ pole je obvykle uint8 (čísla 0 - 255). Pokud k pixelu s hodnotou 250 přičtete 50, nedosáhnete hodnoty 300. Výsledná hodnota "přeteče" (overflow) a výsledkem bude úplně jiná barva (v tomto případě 44).

    Před přičítáním musíte pole převést na větší datový typ (např. int16), provést operaci, oříznout hodnoty nad 255 funkcí np.clip() a vrátit zpět na uint8. Dojde tedy k tzv. saturaci.

5. Černobílý filtr (Grayscale) Chceme z 3D pole RGB udělat 2D pole (jen jas). Výsledkem bude šedá barva. Nejjednodušší přístup je zprůměrovat hodnoty R, G a B pro každý pixel, což je přímočarí řešení, které však neodpovídá realitě, na kterou reaguje lidské oko.

    Lidské oko je nejcitlivější na zelenou a nejméně na modrou barvu. Místo prostého průměru použijte vážený průměr: 0.299*R + 0.587*G + 0.114*B. Převeďte výsledek zpět na uint8.

Testování

    Worker musí bez pádu ustát situaci, kdy dostane zprávu s neplatnou operací, např. {"operation": "expoit-op"}.

    Napište integrační test, který nasimuluje poslání 10 úloh do brokera a ověří, že Worker postupně všechny zpracuje a odešle 10 potvrzovacích zpráv.

    Ideálně si napište velmi jednoducho webovou aplikaci v Reactu nebo jiném frameworku, která vám pomůže vizualizovat obrázky a pomůže s odesíláním pozadavků na vaši službu.

Pravidla odevzdání a AI report

    Kód musí využívat asynchronní programování (async/await).

    AI Report: Opět kratičce popište, jak jste použili AI. A jako bonus explicitně uvádím, že tento report může obsahovat i to, že jste AI nepoužili.
