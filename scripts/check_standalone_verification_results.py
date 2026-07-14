#!/usr/bin/env python3
"""Require a current successful result for every standalone test."""

from __future__ import annotations

import argparse
from pathlib import Path

from standalone_verification_results import current_successes


def expected_tests(root: Path) -> set[str]:
    return {
        path.relative_to(root).as_posix()
        for path in (root / "test" / "standalone").glob("*.test.cpp")
    }


def check_results(root: Path, result_dir: Path) -> list[str]:
    successes, issues = current_successes(root, result_dir)
    expected = expected_tests(root)
    missing = sorted(expected - successes.keys())
    extra = sorted(successes.keys() - expected)
    failures = list(issues)
    failures.extend(f"missing current success: {path}" for path in missing)
    failures.extend(f"unexpected current success: {path}" for path in extra)
    return failures


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("."))
    parser.add_argument(
        "--result-dir",
        type=Path,
        default=Path(".competitive-verifier/cache/standalone-results"),
    )
    args = parser.parse_args()
    root = args.root.resolve()
    result_dir = args.result_dir.resolve()
    failures = check_results(root, result_dir)
    if failures:
        for failure in failures:
            print(failure)
        print(f"standalone result check failed: {len(failures)} violation(s)")
        return 1
    print(f"standalone result check passed: {len(expected_tests(root))} test(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
