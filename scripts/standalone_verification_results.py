#!/usr/bin/env python3
"""Persistent, source-bound results for generated standalone tests."""

from __future__ import annotations

import datetime as dt
import hashlib
import json
import math
import os
import re
import tempfile
from pathlib import Path
from pathlib import PurePosixPath
from typing import Any


SCHEMA_VERSION = 1
VALID_STATUSES = frozenset({"running", "success", "failure"})
SHA256_PATTERN = re.compile(r"^[0-9a-f]{64}$")
SOURCE_SUFFIXES = {".cpp", ".hpp", ".py"}
SOURCE_DIRECTORIES = ("src", "test")
SOURCE_FILES = (
    "Makefile",
    "config.toml",
    "scripts/run_standalone_assets.py",
    "scripts/standalone_checker.py",
    "scripts/standalone_verification_results.py",
)


class StandaloneResultError(ValueError):
    pass


def source_files(root: Path) -> list[Path]:
    root = root.resolve()
    files: set[Path] = set()
    for directory_name in SOURCE_DIRECTORIES:
        directory = root / directory_name
        if not directory.is_dir():
            continue
        for path in directory.rglob("*"):
            if (
                path.is_file()
                and path.suffix in SOURCE_SUFFIXES
                and "__pycache__" not in path.parts
            ):
                files.add(path.resolve())
    for relative in SOURCE_FILES:
        path = (root / relative).resolve()
        if path.is_file():
            files.add(path)
    return sorted(files, key=lambda path: path.relative_to(root).as_posix())


def source_fingerprint(root: Path) -> str:
    root = root.resolve()
    digest = hashlib.sha256()
    digest.update(f"standalone-results-v{SCHEMA_VERSION}\0".encode())
    for path in source_files(root):
        relative = path.relative_to(root).as_posix().encode()
        data = path.read_bytes()
        digest.update(len(relative).to_bytes(8, "big"))
        digest.update(relative)
        digest.update(len(data).to_bytes(8, "big"))
        digest.update(data)
    return digest.hexdigest()


def result_path(result_dir: Path, test_path: str) -> Path:
    parsed = PurePosixPath(test_path)
    if len(parsed.parts) != 3 or parsed.parts[:2] != ("test", "standalone"):
        raise StandaloneResultError(f"not a standalone test path: {test_path}")
    name = parsed.name
    suffix = ".test.cpp"
    if not name.endswith(suffix):
        raise StandaloneResultError(f"not a standalone test path: {test_path}")
    return result_dir / f"{name[:-len(suffix)]}.json"


def make_manifest(
    *,
    test_path: str,
    fingerprint: str,
    status: str,
    elapsed_seconds: float = 0.0,
    case_count: int = 0,
    cxx: str = "",
    cxxflags: str = "",
    executed_at: str | None = None,
) -> dict[str, Any]:
    if status not in VALID_STATUSES:
        raise StandaloneResultError(f"invalid standalone status: {status}")
    if not SHA256_PATTERN.fullmatch(fingerprint):
        raise StandaloneResultError("fingerprint must be a lowercase SHA-256 digest")
    result_path(Path("."), test_path)
    if (
        isinstance(elapsed_seconds, bool)
        or not isinstance(elapsed_seconds, (int, float))
        or not math.isfinite(float(elapsed_seconds))
        or elapsed_seconds < 0
    ):
        raise StandaloneResultError("elapsed_seconds must be finite and non-negative")
    if isinstance(case_count, bool) or not isinstance(case_count, int) or case_count < 0:
        raise StandaloneResultError("case_count must be a non-negative integer")
    if executed_at is None:
        executed_at = dt.datetime.now(dt.timezone.utc).isoformat().replace("+00:00", "Z")
    return {
        "schema_version": SCHEMA_VERSION,
        "test": test_path,
        "fingerprint": fingerprint,
        "status": status,
        "elapsed_seconds": float(elapsed_seconds),
        "case_count": int(case_count),
        "cxx": cxx,
        "cxxflags": cxxflags,
        "executed_at": executed_at,
    }


def write_manifest(path: Path, manifest: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    descriptor, temporary_name = tempfile.mkstemp(
        prefix=f".{path.name}.", suffix=".tmp", dir=path.parent, text=True
    )
    temporary = Path(temporary_name)
    try:
        with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as stream:
            json.dump(manifest, stream, ensure_ascii=False, sort_keys=True)
            stream.write("\n")
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, path)
    except BaseException:
        temporary.unlink(missing_ok=True)
        raise


def load_manifest(path: Path) -> dict[str, Any]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise StandaloneResultError(f"invalid result {path}: {error}") from error
    if not isinstance(value, dict):
        raise StandaloneResultError(f"result is not an object: {path}")
    required = {
        "schema_version": int,
        "test": str,
        "fingerprint": str,
        "status": str,
        "elapsed_seconds": (int, float),
        "case_count": int,
        "executed_at": str,
        "cxx": str,
        "cxxflags": str,
    }
    for key, expected_type in required.items():
        field = value.get(key)
        if not isinstance(field, expected_type) or (
            key in {"elapsed_seconds", "case_count"} and isinstance(field, bool)
        ):
            raise StandaloneResultError(f"invalid {key} in result: {path}")
    if value["schema_version"] != SCHEMA_VERSION:
        raise StandaloneResultError(f"unsupported result schema: {path}")
    if (
        not math.isfinite(float(value["elapsed_seconds"]))
        or value["elapsed_seconds"] < 0
        or value["case_count"] < 0
    ):
        raise StandaloneResultError(f"negative measurement in result: {path}")
    if value["status"] not in VALID_STATUSES:
        raise StandaloneResultError(f"invalid status in result: {path}")
    if not SHA256_PATTERN.fullmatch(value["fingerprint"]):
        raise StandaloneResultError(f"invalid fingerprint in result: {path}")
    try:
        result_path(Path("."), value["test"])
    except StandaloneResultError as error:
        raise StandaloneResultError(f"invalid test in result: {path}") from error
    try:
        timestamp = dt.datetime.fromisoformat(
            value["executed_at"].replace("Z", "+00:00")
        )
    except ValueError as error:
        raise StandaloneResultError(f"invalid executed_at in result: {path}") from error
    if timestamp.tzinfo is None or timestamp.utcoffset() is None:
        raise StandaloneResultError(f"executed_at must include a timezone: {path}")
    return value


def current_successes(root: Path, result_dir: Path) -> tuple[dict[str, dict[str, Any]], list[str]]:
    fingerprint = source_fingerprint(root)
    successes: dict[str, dict[str, Any]] = {}
    issues: list[str] = []
    if not result_dir.is_dir():
        return successes, [f"result directory does not exist: {result_dir}"]
    for path in sorted(result_dir.glob("*.json")):
        try:
            manifest = load_manifest(path)
        except StandaloneResultError as error:
            issues.append(str(error))
            continue
        test_path = manifest["test"]
        try:
            expected_path = result_path(result_dir, test_path)
        except StandaloneResultError as error:
            issues.append(str(error))
            continue
        if expected_path.name != path.name:
            issues.append(f"result filename does not match test: {path}")
            continue
        if manifest["fingerprint"] != fingerprint:
            issues.append(f"stale result: {test_path}")
            continue
        if manifest["status"] != "success":
            issues.append(f"non-success result ({manifest['status']}): {test_path}")
            continue
        if manifest["case_count"] <= 0:
            issues.append(f"zero-case success result: {test_path}")
            continue
        if test_path in successes:
            issues.append(f"duplicate result: {test_path}")
            continue
        successes[test_path] = manifest
    return successes, issues
