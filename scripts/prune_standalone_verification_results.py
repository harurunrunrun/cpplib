#!/usr/bin/env python3
"""Remove result manifests that belong to deleted or renamed standalone tests."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from standalone_verification_results import (
    StandaloneResultError,
    load_manifest,
    result_path,
)


def existing_test_paths(root: Path) -> set[str]:
    root = root.resolve()
    return {
        path.relative_to(root).as_posix()
        for path in (root / "test" / "standalone").glob("*.test.cpp")
        if path.is_file()
    }


def prune_obsolete_results(root: Path, result_dir: Path) -> list[Path]:
    """Delete only valid, correctly named manifests for tests that disappeared."""

    root = root.resolve()
    result_dir = result_dir.resolve()
    if not result_dir.is_dir():
        return []

    current_tests = existing_test_paths(root)
    removed: list[Path] = []
    for path in sorted(result_dir.glob("*.json")):
        try:
            manifest = load_manifest(path)
        except StandaloneResultError:
            # Unknown JSON and malformed/current results must remain visible to
            # the result checker instead of being mistaken for obsolete output.
            continue

        test_path = manifest["test"]
        if test_path in current_tests:
            continue
        if result_path(result_dir, test_path).name != path.name:
            # A mismatched filename is not known to be runner-owned.
            continue

        # glob() only yields direct children. unlink() removes a symlink itself,
        # never the file or directory to which it points.
        if path.is_file() or path.is_symlink():
            path.unlink(missing_ok=True)
            removed.append(path)

    return removed


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path("."))
    parser.add_argument(
        "--result-dir",
        type=Path,
        default=Path(".competitive-verifier/cache/standalone-results"),
    )
    args = parser.parse_args(argv)

    try:
        removed = prune_obsolete_results(args.root, args.result_dir)
    except OSError as error:
        print(
            f"[standalone-results-prune] could not prune obsolete manifests: {error}",
            file=sys.stderr,
        )
        return 1

    for path in removed:
        print(f"[standalone-results-prune] removed {path}", file=sys.stderr)
    print(
        "[standalone-results-prune] "
        f"PASS: removed {len(removed)} obsolete manifest(s)",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
