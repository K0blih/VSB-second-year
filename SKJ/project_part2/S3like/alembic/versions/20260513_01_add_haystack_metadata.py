"""Add Haystack storage metadata to files."""

from __future__ import annotations

from alembic import op
import sqlalchemy as sa


revision = "20260513_01"
down_revision = "20260408_03"
branch_labels = None
depends_on = None


def upgrade() -> None:
    with op.batch_alter_table("files") as batch_op:
        batch_op.add_column(sa.Column("status", sa.String(), nullable=False, server_default="ready"))
        batch_op.add_column(sa.Column("volume_id", sa.Integer(), nullable=True))
        batch_op.add_column(sa.Column("offset", sa.Integer(), nullable=True))
        batch_op.add_column(
            sa.Column("upload_internal_source", sa.Boolean(), nullable=False, server_default=sa.false())
        )


def downgrade() -> None:
    with op.batch_alter_table("files") as batch_op:
        batch_op.drop_column("upload_internal_source")
        batch_op.drop_column("offset")
        batch_op.drop_column("volume_id")
        batch_op.drop_column("status")
