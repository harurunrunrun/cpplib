import json
import sys
from pathlib import Path


def main() -> None:
    with open(sys.argv[1], encoding="utf-8") as file:
        verify_files = json.load(file)

    result_path = Path(sys.argv[2])
    if result_path.is_file():
        with result_path.open(encoding="utf-8") as file:
            result = json.load(file)
    else:
        result = {"total_seconds": 0, "files": {}}

    for path, data in verify_files["files"].items():
        if data["verification"]:
            result["files"].setdefault(
                path,
                {"verifications": [], "newest": False},
            )

    json.dump(result, sys.stdout)
    print()


if __name__ == "__main__":
    main()
