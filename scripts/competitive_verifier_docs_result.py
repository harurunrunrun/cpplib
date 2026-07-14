import argparse
import json
import sys
from pathlib import Path

from standalone_verification_results import current_successes


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


def _is_standalone(data: dict) -> bool:
    attributes = data.get("document_attributes", {})
    return isinstance(attributes, dict) and "STANDALONE" in attributes


def _verification_name(data: dict) -> str | None:
    verifications = data.get("verification", [])
    if not isinstance(verifications, list) or len(verifications) != 1:
        return None
    verification = verifications[0]
    if not isinstance(verification, dict):
        return None
    name = verification.get("name")
    return name if isinstance(name, str) and name else None


def standalone_file_result(manifest: dict, data: dict) -> dict | None:
    verification_name = _verification_name(data)
    if verification_name is None:
        return None
    elapsed = float(manifest["elapsed_seconds"])
    return {
        "verifications": [
            {
                "verification_name": verification_name,
                "status": "success",
                "elapsed": elapsed,
                "slowest": None,
                "heaviest": None,
                "testcases": [
                    {
                        "name": f"{manifest['case_count']} generated case(s)",
                        "status": "AC",
                        "elapsed": elapsed,
                        "memory": None,
                    }
                ],
                "last_execution_time": manifest["executed_at"],
            }
        ],
        "newest": True,
    }


def build_docs_result(
    verify_files: dict,
    online_result: dict,
    *,
    root: Path,
    standalone_results: Path | None,
) -> dict:
    plan_files = verify_files.get("files")
    if not isinstance(plan_files, dict):
        raise ValueError("verification plan must contain a files object")

    planned = {
        path: data
        for path, data in plan_files.items()
        if isinstance(path, str)
        and isinstance(data, dict)
        and isinstance(data.get("verification"), list)
        and bool(data["verification"])
    }
    online_files = online_result.get("files", {})
    if not isinstance(online_files, dict):
        online_files = {}

    files: dict[str, dict] = {}
    for path, data in planned.items():
        value = online_files.get(path)
        if not _is_standalone(data) and isinstance(value, dict):
            files[path] = value

    standalone_elapsed = 0.0
    if standalone_results is not None:
        successes, issues = current_successes(root, standalone_results)
        if issues:
            print(
                f"[docs-result] ignored {len(issues)} stale/failed result(s)",
                file=sys.stderr,
            )
        for path, manifest in successes.items():
            data = planned.get(path)
            if data is None or not _is_standalone(data):
                continue
            value = standalone_file_result(manifest, data)
            if value is None:
                continue
            files[path] = value
            standalone_elapsed += float(manifest["elapsed_seconds"])

    for path in planned:
        files.setdefault(path, {"verifications": [], "newest": False})

    total_seconds = online_result.get("total_seconds", 0)
    if isinstance(total_seconds, bool) or not isinstance(total_seconds, (int, float)):
        total_seconds = 0
    return {
        "total_seconds": float(total_seconds) + standalone_elapsed,
        "files": files,
    }


def main(argv: list[str] | None = None) -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("verify_files", type=Path)
    parser.add_argument("online_result", type=Path)
    parser.add_argument("--standalone-results", type=Path)
    parser.add_argument("--root", type=Path, default=Path("."))
    args = parser.parse_args(argv)

    with args.verify_files.open(encoding="utf-8") as file:
        verify_files = json.load(file)
    result = build_docs_result(
        verify_files,
        load_result(args.online_result),
        root=args.root.resolve(),
        standalone_results=(
            args.standalone_results.resolve()
            if args.standalone_results is not None
            else None
        ),
    )
    json.dump(result, sys.stdout)
    print()


if __name__ == "__main__":
    main()
