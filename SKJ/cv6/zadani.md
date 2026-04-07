Perzistence a validace dat

Na základě implementace z předcházejícího cvičení rozšíříme službu do podoby pokročilejšího object storage systému.

Na tomto cvičení si kážeme perzistentní vrstvu v podobě SQL knihovny SQLAlchemy. A validaci vstupů pomocí knihovny Pydantic.
Perzistence s Object Relational Mapping (ORM)

ORM je programovací technika, která automaticky převádí data mezi relační databází a objektově orientovaným kódem. Existuje spousta knihoven pro ORM v různých jazycích, např. Entitiy Framework v C#, Hibernate.

V Pythonu existuje opět několik implementací ORM. K nejznámějším se řadí právě SQLAlchemy nebo ORM vrstva ve frameworku Django.

ORM nám zajištujě to, že nemusíme psát přímo SQL dotazy v Python kódu. Dotazy do DB se překládají automaticky za použití speciálních tříd (tzv. modely), které definují DB tabulky. Instance takových tříd jsou pak jednotlivými řádky (velmi zjednodušeně řečeno a platí to pro návrhový vzor Active Record).

Pro základní práci s DB si můžeme nadefinovat soubor database.py:

from sqlalchemy import create_engine
from sqlalchemy.orm import DeclarativeBase

SQLALCHEMY_DATABASE_URL = "sqlite:///./todo.db"

engine = create_engine(SQLALCHEMY_DATABASE_URL, connect_args={"check_same_thread": False}, echo=True)

class Base(DeclarativeBase):
    pass

Z SQLALCHEMY_DATABASE_URL je patrné, že budeme používat SQLite, proměnná engine pak reprezentuje připojení do DB (parametr echo=True zajistí, že v konzoli uvidíme tzv. raw SQL příkazy). Námi defonované DB modely pak budou dědit ze třídy Base, která dědí z obecné třídy DeclarativeBase z SQLAlchemy.

Poté můžeme definovat vlastní DB modely např. v souboru models.py:

from database import Base
from sqlalchemy import String, Boolean
from sqlalchemy.orm import Mapped, mapped_column

class Task(Base):
    __tablename__ = "tasks"

    id: Mapped[int] = mapped_column(primary_key=True, autoincrement=True)
    title: Mapped[str] = mapped_column(String(50))
    description: Mapped[str] = mapped_column(String, nullable=True)
    completed: Mapped[bool] = mapped_column(Boolean, default=False)

    def __repr__(self) -> str:
        return f"Task(id={self.id}, title={self.title}, description={self.description}, completed={self.completed})"

Naše třída Task dědí z Base. __tablename__ specifikuje jméno tabulky v SQL databázi. Jednotlivé atributy třídy se mapují na sloupce v tabulce. Datový typ je určen pomocí type hintů Mapped[typ] a následným použitím mapped_column s příslušnými parametry. Pak se např. typ String(50) přeloží v na DB typ VARCHAR(50).

V main.py musíme vytvořit naši DB:

from database import engine
import models

models.Base.metadata.create_all(bind=engine)

def get_db():
    db = Session(bind=engine.connect())
    try:
        yield db
    finally:
        db.close()

Můžeme pak vytvořit záznam do DB v našem FastAPI endpointu např. takto:

@app.post("/db-tasks/")
def create_db_task(task, db: Session = Depends(get_db)):
    db_task = models.Task(title=task.title, description=task.description, completed=task.completed)
    db.add(db_task)
    db.commit()
    db.refresh(db_task)

A nebo také vyhledávat:

@app.get("/db-tasks/")
def get_db_tasks(db: Session = Depends(get_db)):
    return db.query(models.Task).all()

Zadání do našeho S3 uložiště

Do svého aktuálního řešení naimplementujte ukládání metadat do databáze místo do JSON souboru.
Validace dat - Pydantic

Vzhledem k dynamické typovosti jazyka Python je častým řešením speciálně návratových hodnot používat slovníky různé "složitosti" a pak to dopadá třeba takto :-(.

Řešením je striktní používání type hintů, které jsou sice v klasickém Pythhonu jeho runtime ignorovány, ale s využitím knohovny jako je Pydantic je možné je kontrolovat za běhu a vyvolávat tak chyby.

from pydantic import BaseModel, Field


class TaskCreate(BaseModel):
    title: str = Field(..., title="Task Title", description="The title of the task",
                       min_length=3, max_length=50)

    description: str | None = Field(
        None, title="Task Description",
        description="The description of the task")

    completed: bool = Field(
        False,
        title="Task Completed",
        description="Whether the task is completed or not")

    model_config = {
        "json_schema_extra": {
            "example": {
                "title": "Buy groceries",
                "description": "Milk, Bread, Eggs",
                "completed": False
            }
        }
    }

Zde definujeme náš typ TaskCreate, který dědí z Pydantic modelu BaseModel. Třída pak obsahuje atributy, které jsou anotovány standartnímy typy z Pythonu a je jim přiřazena hodnota Field z Pydanticu, která zajistí dané omezení na zvolený datový typ.

Když takovou třídu požijeme v našem endpointu, všimněte si, že se toto projeví i v lokální dokumentaci k danému endpointu:

@app.post("/db-tasks/", response_model=TaskCreate, tags=["db-tasks"],
          summary="Create a new task in the database",
          response_description="The created task")
def create_db_task(task: TaskCreate, db: Session = Depends(get_db)):
    db_task = models.Task(title=task.title, description=task.description, completed=task.completed)
    db.add(db_task)
    db.commit()
    db.refresh(db_task)
    return db_task

Konkrétně se náš Pydantic model objevuje v parametru response_model a také jako type hint přímo v parametru funkce create_db_task.
Zadání pro validaci dat

Pro všechny vstupy a výstupy, tedy parametry endpointů a návratové hodnoty z nich, používejte Pydantic modely.

Nadefinujte si jednotlivé modely pro requesty i response a nepoužívejte jen obyčejné "raw" slovníky (dict) jako návratové hodnoty.
AI report

Součástí odevzdání bude krátký report ve formátu Markdown, který bude obsahovat:

    jaké nástroje AI byly použity
    příklady promptů
    co AI vygenerovala správně
    co bylo nutné opravit
    jaké chyby AI udělala
