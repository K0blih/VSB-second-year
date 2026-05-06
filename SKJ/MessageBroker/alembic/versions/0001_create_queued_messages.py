"""create queued messages

Revision ID: 0001_create_queued_messages
Revises:
Create Date: 2026-04-16
"""
from alembic import op
import sqlalchemy as sa


revision = "0001_create_queued_messages"
down_revision = None
branch_labels = None
depends_on = None


def upgrade() -> None:
    op.create_table(
        "queued_messages",
        sa.Column("id", sa.Integer(), nullable=False),
        sa.Column("topic", sa.String(length=255), nullable=False),
        sa.Column("payload", sa.LargeBinary(), nullable=False),
        sa.Column("format", sa.String(length=20), nullable=False),
        sa.Column("created_at", sa.DateTime(timezone=True), server_default=sa.func.now(), nullable=False),
        sa.Column("is_delivered", sa.Boolean(), nullable=False),
        sa.PrimaryKeyConstraint("id"),
    )
    op.create_index(op.f("ix_queued_messages_id"), "queued_messages", ["id"], unique=False)
    op.create_index(op.f("ix_queued_messages_topic"), "queued_messages", ["topic"], unique=False)
    op.create_index(op.f("ix_queued_messages_is_delivered"), "queued_messages", ["is_delivered"], unique=False)


def downgrade() -> None:
    op.drop_index(op.f("ix_queued_messages_is_delivered"), table_name="queued_messages")
    op.drop_index(op.f("ix_queued_messages_topic"), table_name="queued_messages")
    op.drop_index(op.f("ix_queued_messages_id"), table_name="queued_messages")
    op.drop_table("queued_messages")
