from datetime import datetime

from sqlalchemy import Boolean, DateTime, ForeignKey, Integer, String, UniqueConstraint
from sqlalchemy.orm import Mapped, mapped_column, relationship

from app.database import Base


class Bucket(Base):
    __tablename__ = "buckets"
    __table_args__ = (UniqueConstraint("owner_id", "name", name="uq_buckets_owner_name"),)

    id: Mapped[int] = mapped_column(Integer, primary_key=True, autoincrement=True)
    owner_id: Mapped[str] = mapped_column(String, index=True, nullable=False)
    name: Mapped[str] = mapped_column(String, nullable=False)
    created_at: Mapped[datetime] = mapped_column(
        DateTime, default=datetime.utcnow, nullable=False
    )
    bandwidth_bytes: Mapped[int] = mapped_column(Integer, default=0, nullable=False)
    current_storage_bytes: Mapped[int] = mapped_column(Integer, default=0, nullable=False)
    ingress_bytes: Mapped[int] = mapped_column(Integer, default=0, nullable=False)
    egress_bytes: Mapped[int] = mapped_column(Integer, default=0, nullable=False)
    internal_transfer_bytes: Mapped[int] = mapped_column(Integer, default=0, nullable=False)
    count_write_requests: Mapped[int] = mapped_column(Integer, default=0, nullable=False)
    count_read_requests: Mapped[int] = mapped_column(Integer, default=0, nullable=False)

    files: Mapped[list["File"]] = relationship(back_populates="bucket")


class File(Base):
    __tablename__ = "files"

    id: Mapped[str] = mapped_column(String, primary_key=True)
    user_id: Mapped[str] = mapped_column(String, index=True, nullable=False)
    bucket_id: Mapped[int] = mapped_column(ForeignKey("buckets.id"), index=True, nullable=False)
    filename: Mapped[str] = mapped_column(String, nullable=False)
    path: Mapped[str] = mapped_column(String, nullable=False, unique=True)
    size: Mapped[int] = mapped_column(Integer, nullable=False)
    status: Mapped[str] = mapped_column(String, default="uploading", nullable=False)
    volume_id: Mapped[int | None] = mapped_column(Integer, nullable=True)
    offset: Mapped[int | None] = mapped_column(Integer, nullable=True)
    upload_internal_source: Mapped[bool] = mapped_column(Boolean, default=False, nullable=False)
    is_deleted: Mapped[bool] = mapped_column(Boolean, default=False, nullable=False)
    created_at: Mapped[datetime] = mapped_column(
        DateTime, default=datetime.utcnow, nullable=False
    )

    bucket: Mapped[Bucket] = relationship(back_populates="files")
