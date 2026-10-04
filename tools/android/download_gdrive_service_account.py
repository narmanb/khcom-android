#!/usr/bin/env python3
import json
import os
import sys
from pathlib import Path

from google.auth.transport.requests import AuthorizedSession
from google.oauth2 import service_account

SCOPE = "https://www.googleapis.com/auth/drive.readonly"

def main():
    if len(sys.argv) != 3:
        sys.exit("usage: download_gdrive_service_account.py FILE_ID OUTPUT_PATH")

    file_id = sys.argv[1].strip()
    output = Path(sys.argv[2])
    raw = os.environ.get("KHCOM_GDRIVE_SERVICE_ACCOUNT_JSON", "")
    if not raw:
        sys.exit("error: KHCOM_GDRIVE_SERVICE_ACCOUNT_JSON is not set")

    try:
        info = json.loads(raw)
    except json.JSONDecodeError as exc:
        sys.exit(f"error: invalid service-account JSON: {exc}")

    creds = service_account.Credentials.from_service_account_info(info, scopes=[SCOPE])
    session = AuthorizedSession(creds)
    url = f"https://www.googleapis.com/drive/v3/files/{file_id}"
    response = session.get(url, params={"alt": "media", "supportsAllDrives": "true"}, stream=True)
    if response.status_code != 200:
        sys.exit(f"error: Google Drive download failed: HTTP {response.status_code}: {response.text[:500]}")

    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open("wb") as f:
        for chunk in response.iter_content(chunk_size=1024 * 1024):
            if chunk:
                f.write(chunk)

    print(f"downloaded {output} ({output.stat().st_size} bytes)")

if __name__ == "__main__":
    main()
