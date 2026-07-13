#!/usr/bin/env python3
"""Remove result entries which are no longer executable in a verifier plan."""

from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Any

from competitive_verifier.models import VerifyCommandResult


class ResultFilterError(ValueError):
    """The verification plan cannot be used to filter a result."""


def _relative_path(path: str | Path, *, root: Path) -> Path:
    """Return PATH in the same canonical, repository-relative form as verifier."""

    candidate = Path(path)
    if not candidate.is_absolute():
        candidate = root / candidate
    try:
        return candidate.resolve().relative_to(root.resolve())
    except ValueError as error:
        raise ResultFilterError(
            f"verification path is outside the repository: {path}"
        ) from error


def planned_verification_paths(
    plan: object, *, root: Path | None = None
) -> set[Path]:
    """Return canonical paths having at least one verification in PLAN."""

    if not isinstance(plan, dict):
        raise ResultFilterError("verification plan must be a JSON object")
    files = plan.get("files")
    if not isinstance(files, dict):
        raise ResultFilterError("verification plan must contain a files object")

    repository = (Path.cwd() if root is None else root).resolve()
    planned: set[Path] = set()
    for path, file_data in files.items():
        if not isinstance(path, str) or not isinstance(file_data, dict):
            raise ResultFilterError("each files entry must map a path to an object")
        verifications = file_data.get("verification", [])
        if not isinstance(verifications, list):
            raise ResultFilterError(f"{path}: verification must be an array")
        if verifications:
            planned.add(_relative_path(path, root=repository))
    return planned


def read_plan_paths(path: Path, *, root: Path | None = None) -> set[Path]:
    with path.open(encoding="utf-8") as stream:
        plan: Any = json.load(stream)
    return planned_verification_paths(plan, root=root)


def filter_result(
    result: VerifyCommandResult,
    planned_paths: set[Path],
    *,
    root: Path | None = None,
) -> VerifyCommandResult:
    """Keep partial results, but only for files still executable in the plan."""

    repository = (Path.cwd() if root is None else root).resolve()
    files = {}
    for path, file_result in result.files.items():
        relative = _relative_path(path, root=repository)
        if relative in planned_paths:
            files[relative] = file_result
    return VerifyCommandResult(total_seconds=result.total_seconds, files=files)


def read_result(path: Path) -> VerifyCommandResult:
    return VerifyCommandResult.parse_file_relative(path)


def write_result_atomic(path: Path, result: VerifyCommandResult) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f".{path.name}.{os.getpid()}.tmp")
    try:
        temporary.write_text(
            result.model_dump_json(exclude_none=True) + "\n",
            encoding="utf-8",
        )
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--verify-json", type=Path, required=True)
    parser.add_argument("--result", type=Path, required=True)
    parser.add_argument(
        "--output",
        type=Path,
        help="output path (default: replace RESULT atomically)",
    )
    args = parser.parse_args(argv)

    destination = args.result if args.output is None else args.output
    try:
        planned = read_plan_paths(args.verify_json)
        result = read_result(args.result)
        filtered = filter_result(result, planned)
        write_result_atomic(destination, filtered)
    except (OSError, UnicodeError, json.JSONDecodeError, ResultFilterError, ValueError) as error:
        print(f"verification result filtering failed: {error}", file=sys.stderr)
        return 1

    removed = len(result.files) - len(filtered.files)
    print(
        "verification result filtering: "
        f"kept {len(filtered.files)}/{len(result.files)} result(s); "
        f"removed {removed} stale result(s)",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
