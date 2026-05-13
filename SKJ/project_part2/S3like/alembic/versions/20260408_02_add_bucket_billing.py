"""Add billing counters to buckets."""

from __future__ import annotations

from alembic import op
import sqlalchemy as sa


revision = "20260408_02"
down_revision = "20260408_01"
branch_labels = None
depends_on = None


def upgrade() -> None:
    with op.batch_alter_table("buckets") as batch_op:
        batch_op.add_column(sa.Column("bandwidth_bytes", sa.Integer(), nullable=False, server_default="0"))
        batch_op.add_column(sa.Column("current_storage_bytes", sa.Integer(), nullable=False, server_default="0"))
        batch_op.add_column(sa.Column("ingress_bytes", sa.Integer(), nullable=False, server_default="0"))
        batch_op.add_column(sa.Column("egress_bytes", sa.Integer(), nullable=False, server_default="0"))
        batch_op.add_column(
            sa.Column("internal_transfer_bytes", sa.Integer(), nullable=False, server_default="0")
        )
        batch_op.add_column(sa.Column("count_write_requests", sa.Integer(), nullable=False, server_default="0"))
        batch_op.add_column(sa.Column("count_read_requests", sa.Integer(), nullable=False, server_default="0"))

    connection = op.get_bind()
    connection.execute(
        sa.text(
            """
            UPDATE buckets
            SET current_storage_bytes = COALESCE(
                (SELECT SUM(size) FROM files WHERE files.bucket_id = buckets.id),
                0
            )
            """
        )
    )


def downgrade() -> None:
    with op.batch_alter_table("buckets") as batch_op:
        batch_op.drop_column("count_read_requests")
        batch_op.drop_column("count_write_requests")
        batch_op.drop_column("internal_transfer_bytes")
        batch_op.drop_column("egress_bytes")
        batch_op.drop_column("ingress_bytes")
        batch_op.drop_column("current_storage_bytes")
        batch_op.drop_column("bandwidth_bytes")
