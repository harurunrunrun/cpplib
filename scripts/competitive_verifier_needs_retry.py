#!/usr/bin/env python3
"""Decide whether a competitive-verifier shard attempt must be retried.

competitive-verifier reports ``is-success: true`` when the time budget runs out
and the remaining files are recorded as ``skipped``, because a skip is not a
failure. Those files are still unverified, so the merged check later fails.
This script inspects the attempt's result file and requests a retry whenever
any verification is not ``success`` (or the result is missing or invalid).
"""

from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path


def needs_retry(result_path: Path, is_success: str) -> tuple[bool, str]:
    """Return whether to retry and a short human-readable reason."""

    if is_success.strip().lower() != "true":
        return True, "the verify action did not report success"
    try:
        data = json.loads(result_path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        return True, f"{result_path} does not exist"
    except (OSError, UnicodeError, ValueError) as error:
        return True, f"{result_path} is not valid JSON: {error}"

    files = data.get("files") if isinstance(data, dict) else None
    if not isinstance(files, dict):
        return True, f"{result_path} has no 'files' object"

    unfinished: list[str] = []
    for name, entry in files.items():
        verifications = entry.get("verifications") if isinstance(entry, dict) else None
        if not isinstance(verifications, list):
            unfinished.append(name)
            continue
        if any(
            not isinstance(verification, dict)
            or verification.get("status") != "success"
            for verification in verifications
        ):
            unfinished.append(name)

    if unfinished:
        preview = ", ".join(sorted(unfinished)[:5])
        more = "" if len(unfinished) <= 5 else f" (+{len(unfinished) - 5} more)"
        return True, f"{len(unfinished)} unfinished file(s): {preview}{more}"
    return False, "every verification succeeded"


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--result", type=Path, required=True)
    parser.add_argument(
        "--is-success",
        default="",
        help="the verify action's is-success output",
    )
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    retry, reason = needs_retry(arguments.result, arguments.is_success)
    print(f"needs_retry={str(retry).lower()}: {reason}")
    output = os.environ.get("GITHUB_OUTPUT")
    if output:
        with Path(output).open("a", encoding="utf-8") as file:
            file.write(f"needs_retry={str(retry).lower()}\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
