# AI Report

## Used Tools
- Codex / ChatGPT

## Example Prompts
- "How do I connect Alembic to a FastAPI project that stores SQLAlchemy models in `app/models.py`?"
- "Show an Alembic `env.py` that imports `Base.metadata` from the app package."
- "How should I migrate an existing `files` table so every old row gets assigned to a new bucket without deleting data?"
- "What usually breaks when Alembic autogenerate handles SQLite foreign keys and relationship changes?"

## What AI Helped With
- The expected Alembic layout: `alembic.ini`, `alembic/env.py`, and versioned migration files.
- The need to import the application metadata in `alembic/env.py`.
- The recommendation to enable `render_as_batch=True` for SQLite schema updates.
- The high-level migration order for buckets, billing counters, and soft delete.

## What Needed Manual Correction
- `env.py` had to add the project root to `sys.path` and import the app models explicitly, otherwise Alembic would not see the metadata correctly.
- The first migration needed custom backfill logic. Autogenerate can add `bucket_id`, but it will not invent safe bucket assignments for existing file rows.
- The original app created tables with `Base.metadata.create_all()` on startup; that had to be removed so Alembic remains the schema source of truth.
- Multipart upload validation still needed to pass through Pydantic while staying compatible with FastAPI form handling.

## Mistakes AI Commonly Made Here
- Importing only `Base` in Alembic without importing the module where models are actually declared, which can produce empty migrations.
- Forgetting SQLite batch mode for `ALTER TABLE` style changes.
- Treating the new `bucket_id` foreign key as a pure schema change and ignoring older data already stored in `files`.
- Assuming ORM relationships automatically solve migration ordering and data backfill requirements.
