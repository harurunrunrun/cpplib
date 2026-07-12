import json
import sys


def main() -> None:
    with open(sys.argv[1], encoding="utf-8") as file:
        verify_files = json.load(file)

    files = {
        path: {"verifications": [], "newest": False}
        for path, data in verify_files["files"].items()
        if data["verification"]
    }
    json.dump({"total_seconds": 0, "files": files}, sys.stdout)
    print()


if __name__ == "__main__":
    main()
