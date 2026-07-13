import json
import sys
from pathlib import Path


def empty_result() -> dict:
    return {"total_seconds": 0, "files": {}}


def load_result(path: Path) -> dict:
    if not path.is_file():
        return empty_result()
    try:
        with path.open(encoding="utf-8") as file:
            result = json.load(file)
    except (OSError, UnicodeError, json.JSONDecodeError):
        return empty_result()
    if not isinstance(result, dict) or not isinstance(result.get("files"), dict):
        return empty_result()
    result.setdefault("total_seconds", 0)
    return result


def main() -> None:
    with open(sys.argv[1], encoding="utf-8") as file:
        verify_files = json.load(file)

    result = load_result(Path(sys.argv[2]))

    for path, data in verify_files["files"].items():
        if data.get("verification"):
            result["files"].setdefault(
                path,
                {"verifications": [], "newest": False},
            )

    json.dump(result, sys.stdout)
    print()


if __name__ == "__main__":
    main()
