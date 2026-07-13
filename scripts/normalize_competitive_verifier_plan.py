#!/usr/bin/env python3
"""Remove known-unresolvable AtCoder entries from a verifier plan.

competitive-verifier 4.1.2 does not provide an AtCoder problem provider. Its
resolver therefore represents an AtCoder PROBLEM as a constant failed
verification. Those entries are not test executions and must not make the
verification job fail. This script removes only that exact representation;
all executable problem verifications are preserved, and an unexpected
constant failure aborts normalization instead of being hidden.
"""

from __future__ import annotations

import argparse
import copy
import json
import os
import sys
import tempfile
from pathlib import Path
from typing import Any
from urllib.parse import urlsplit


class PlanNormalizationError(ValueError):
    """The verifier plan has an unexpected or malformed entry."""


def is_atcoder_url(value: object) -> bool:
    if not isinstance(value, str):
        return False
    try:
        parsed = urlsplit(value)
        hostname = parsed.hostname
    except ValueError:
        return False
    if parsed.scheme not in {"http", "https"} or hostname is None:
        return False
    hostname = hostname.rstrip(".").lower()
    return hostname == "atcoder.jp" or hostname.endswith(".atcoder.jp")


def _documented_problem(file_data: dict[str, Any]) -> object:
    attributes = file_data.get("document_attributes", {})
    if attributes is not None and not isinstance(attributes, dict):
        raise PlanNormalizationError("document_attributes must be an object")
    if isinstance(attributes, dict):
        return attributes.get("PROBLEM")
    return None


def _is_unresolved_atcoder_provider_entry(
    file_data: dict[str, Any], verification: dict[str, Any]
) -> bool:
    # competitive-verifier 4.1.2 emits exactly this minimal constant failure
    # when problem_from_url cannot find a provider. Requiring exact equality
    # prevents an unrelated or future richer failure from being hidden.
    return verification == {"type": "const", "status": "failure"} and is_atcoder_url(
        _documented_problem(file_data)
    )


def normalize_plan(plan: object) -> tuple[dict[str, Any], list[str]]:
    """Return a normalized deep copy and paths whose AtCoder failures moved out."""

    if not isinstance(plan, dict):
        raise PlanNormalizationError("verification plan must be a JSON object")
    files = plan.get("files")
    if not isinstance(files, dict):
        raise PlanNormalizationError("verification plan must contain a files object")

    normalized = copy.deepcopy(plan)
    normalized_files = normalized["files"]
    removed_paths: list[str] = []
    unexpected: list[str] = []

    for path, file_data in normalized_files.items():
        if not isinstance(path, str) or not isinstance(file_data, dict):
            raise PlanNormalizationError("each files entry must map a path to an object")
        verifications = file_data.get("verification", [])
        if not isinstance(verifications, list):
            raise PlanNormalizationError(f"{path}: verification must be an array")

        kept: list[dict[str, Any]] = []
        removed_count = 0
        for verification in verifications:
            if not isinstance(verification, dict):
                raise PlanNormalizationError(f"{path}: verification entry must be an object")
            is_constant_failure = (
                verification.get("type") == "const"
                and verification.get("status") == "failure"
            )
            if not is_constant_failure:
                kept.append(verification)
                continue

            documented_problem = _documented_problem(file_data)
            if _is_unresolved_atcoder_provider_entry(file_data, verification):
                removed_count += 1
                continue

            rendered_urls = (
                repr(documented_problem) if documented_problem else "no PROBLEM URL"
            )
            unexpected.append(f"{path}: {rendered_urls}")
            kept.append(verification)

        if removed_count:
            file_data["verification"] = kept
            removed_paths.extend([path] * removed_count)

    if unexpected:
        details = "\n  - ".join(unexpected)
        raise PlanNormalizationError(
            "unexpected constant verification failure(s); refusing to hide them:\n"
            f"  - {details}"
        )
    return normalized, removed_paths


def _write_json_atomic(path: Path, data: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    descriptor, temporary_name = tempfile.mkstemp(
        prefix=f".{path.name}.",
        suffix=".tmp",
        dir=path.parent,
        text=True,
    )
    temporary = Path(temporary_name)
    try:
        with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as stream:
            json.dump(data, stream, ensure_ascii=False, separators=(",", ":"))
            stream.write("\n")
        os.replace(temporary, path)
    except BaseException:
        temporary.unlink(missing_ok=True)
        raise


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("plan", type=Path, help="competitive-verifier verify-files JSON")
    parser.add_argument(
        "--output",
        type=Path,
        help="output path (default: replace PLAN atomically)",
    )
    args = parser.parse_args(argv)

    try:
        with args.plan.open(encoding="utf-8") as stream:
            source = json.load(stream)
        normalized, removed = normalize_plan(source)
        destination = args.output if args.output is not None else args.plan
        _write_json_atomic(destination, normalized)
    except (OSError, UnicodeError, json.JSONDecodeError, PlanNormalizationError) as error:
        print(f"verification plan normalization failed: {error}", file=sys.stderr)
        return 1

    unique_paths = list(dict.fromkeys(removed))
    print(
        "verification plan normalization: "
        f"removed {len(removed)} unresolved AtCoder verification(s) "
        f"from {len(unique_paths)} file(s)",
        file=sys.stderr,
    )
    for path in unique_paths:
        print(f"  - {path}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
