"""Add buckets and assign existing files to default buckets."""

from __future__ import annotations

from datetime import datetime

from alembic import op
import sqlalchemy as sa


revision = "20260408_01"
down_revision = None
branch_labels = None
depends_on = None


def upgrade() -> None:
    connection = op.get_bind()
    inspector = sa.inspect(connection)

    if not inspector.has_table("files"):
        op.create_table(
            "files",
            sa.Column("id", sa.String(), primary_key=True),
            sa.Column("user_id", sa.String(), nullable=False),
            sa.Column("filename", sa.String(), nullable=False),
            sa.Column("path", sa.String(), nullable=False),
            sa.Column("size", sa.Integer(), nullable=False),
            sa.Column("created_at", sa.DateTime(), nullable=False),
            sa.UniqueConstraint("path"),
        )
        op.create_index("ix_files_user_id", "files", ["user_id"])

    op.create_table(
        "buckets",
        sa.Column("id", sa.Integer(), primary_key=True, autoincrement=True),
        sa.Column("owner_id", sa.String(), nullable=False),
        sa.Column("name", sa.String(), nullable=False),
        sa.Column("created_at", sa.DateTime(), nullable=False),
        sa.UniqueConstraint("owner_id", "name", name="uq_buckets_owner_name"),
    )
    op.create_index("ix_buckets_owner_id", "buckets", ["owner_id"])

    with op.batch_alter_table("files") as batch_op:
        batch_op.add_column(sa.Column("bucket_id", sa.Integer(), nullable=True))

    metadata = sa.MetaData()
    files = sa.Table(
        "files",
        metadata,
        sa.Column("id", sa.String()),
        sa.Column("user_id", sa.String()),
        sa.Column("size", sa.Integer()),
        sa.Column("bucket_id", sa.Integer()),
    )
    buckets = sa.Table(
        "buckets",
        metadata,
        sa.Column("id", sa.Integer()),
        sa.Column("owner_id", sa.String()),
        sa.Column("name", sa.String()),
        sa.Column("created_at", sa.DateTime()),
    )

    existing_users = [row[0] for row in connection.execute(sa.select(sa.distinct(files.c.user_id)))]
    for user_id in existing_users:
        connection.execute(
            buckets.insert().values(owner_id=user_id, name="default", created_at=datetime.utcnow())
        )
        bucket_id = connection.execute(
            sa.select(buckets.c.id)
            .where(buckets.c.owner_id == user_id, buckets.c.name == "default")
            .order_by(buckets.c.id.desc())
            .limit(1)
        ).scalar_one()
        connection.execute(
            files.update().where(files.c.user_id == user_id).values(bucket_id=bucket_id)
        )

    with op.batch_alter_table("files") as batch_op:
        batch_op.alter_column("bucket_id", existing_type=sa.Integer(), nullable=False)
        batch_op.create_index("ix_files_bucket_id", ["bucket_id"])
        batch_op.create_foreign_key("fk_files_bucket_id", "buckets", ["bucket_id"], ["id"])


def downgrade() -> None:
    with op.batch_alter_table("files") as batch_op:
        batch_op.drop_constraint("fk_files_bucket_id", type_="foreignkey")
        batch_op.drop_index("ix_files_bucket_id")
        batch_op.drop_column("bucket_id")

    op.drop_index("ix_buckets_owner_id", table_name="buckets")
    op.drop_table("buckets")
    op.drop_index("ix_files_user_id", table_name="files")
    op.drop_table("files")
