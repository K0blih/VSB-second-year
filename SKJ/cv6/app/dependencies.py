import re

from fastapi import Header, HTTPException, status


USER_ID_PATTERN = re.compile(r"^[A-Za-z0-9_-]+$")


def get_current_user_id(x_user_id: str | None = Header(default=None)) -> str:
    if not x_user_id:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="X-User-Id header is required.",
        )
    if not USER_ID_PATTERN.fullmatch(x_user_id):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="X-User-Id may only contain letters, numbers, underscores, and hyphens.",
        )
    return x_user_id
