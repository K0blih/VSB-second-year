# AI Report

## Used Tools
- Codex / ChatGPT

## Example Prompts
- "Create a FastAPI object storage service with upload, list, download, and delete endpoints."
- "Show how to store uploaded files on disk and persist metadata with SQLAlchemy."
- "Refactor the FastAPI app so metadata is stored in SQLite with SQLAlchemy and API inputs/outputs use Pydantic models."

## What AI Generated Correctly
- Project scaffold
- Basic FastAPI route structure
- SQLAlchemy model shape
- Initial storage flow for saving uploaded files on disk

## What Needed Fixing
- Add project-specific access control with `X-User-Id`
- Adjust storage layout to isolate users
- Improve error handling for disk/database consistency
- Add explicit Pydantic models for documented JSON responses and validated request data

## Mistakes Made by AI
- It initially relied on ad-hoc validation logic instead of consistent schema-based validation.
- Some responses were returned as raw dictionaries rather than explicit response models.
- The generated implementation did not fully describe assignment-specific details in the documentation/report.
