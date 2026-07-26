#!/usr/bin/env python3
"""Reject quoted C++ includes that do not resolve inside the repository."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

QUOTED_INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"]+)"')
CPP_SUFFIXES = frozenset({".cpp", ".hpp"})


def relative_include_violations(
    repository_root: Path,
    scan_roots: list[Path],
) -> list[str]:
    """Return unresolved quoted includes below *scan_roots*."""

    root = repository_root.resolve()
    errors: list[str] = []
    files = sorted(
        path
        for scan_root in scan_roots
        for path in (root / scan_root).rglob("*")
        if path.is_file() and path.suffix in CPP_SUFFIXES
    )
    for path in files:
        try:
            lines = path.read_text(encoding="utf-8-sig").splitlines()
        except (OSError, UnicodeError) as error:
            errors.append(
                f"{path.relative_to(root).as_posix()}: cannot read as UTF-8: {error}"
            )
            continue
        for line_number, line in enumerate(lines, start=1):
            match = QUOTED_INCLUDE.match(line)
            if match is None:
                continue
            include = Path(match.group(1))
            candidates = (
                (path.parent / include).resolve(),
                (root / include).resolve(),
            )
            if any(
                candidate.is_relative_to(root) and candidate.is_file()
                for candidate in candidates
            ):
                continue
            errors.append(
                f"{path.relative_to(root).as_posix()}:{line_number}: "
                f"quoted include does not resolve: {include.as_posix()}"
            )
    return errors


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="check that quoted C++ includes resolve inside the repository"
    )
    parser.add_argument("roots", nargs="*", type=Path, default=[Path("src"), Path("test")])
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    root = Path.cwd()
    errors = relative_include_violations(root, arguments.roots)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        print(
            f"relative include check failed: {len(errors)} violation(s)",
            file=sys.stderr,
        )
        return 1
    print("relative include check passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
