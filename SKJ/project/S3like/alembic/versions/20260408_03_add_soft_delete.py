"""Add soft delete flag to files."""

from __future__ import annotations

from alembic import op
import sqlalchemy as sa


revision = "20260408_03"
down_revision = "20260408_02"
branch_labels = None
depends_on = None


def upgrade() -> None:
    with op.batch_alter_table("files") as batch_op:
        batch_op.add_column(
            sa.Column("is_deleted", sa.Boolean(), nullable=False, server_default=sa.false())
        )


def downgrade() -> None:
    with op.batch_alter_table("files") as batch_op:
        batch_op.drop_column("is_deleted")
