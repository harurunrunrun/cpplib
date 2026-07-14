#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


LOCAL_INCLUDE = re.compile(r'^\s*#include\s+"([^"]+)"', re.MULTILINE)


def local_dependencies(path: Path, root: Path) -> list[Path]:
    try:
        source = path.read_text(encoding="utf-8")
    except (OSError, UnicodeError):
        return []

    dependencies: list[Path] = []
    for include in LOCAL_INCLUDE.findall(source):
        candidates = (path.parent / include, root / include)
        for candidate in candidates:
            resolved = candidate.resolve()
            try:
                resolved.relative_to(root)
            except ValueError:
                continue
            if resolved.is_file():
                dependencies.append(resolved)
                break
    return dependencies


def headers_targeted_by_tests(root: Path) -> set[Path]:
    root = root.resolve()
    stack = [
        path.resolve()
        for path in (root / "test").rglob("*.test.cpp")
    ]
    visited: set[Path] = set()
    targeted: set[Path] = set()
    while stack:
        path = stack.pop()
        if path in visited:
            continue
        visited.add(path)
        try:
            relative = path.relative_to(root)
        except ValueError:
            continue
        for dependency in local_dependencies(path, root):
            try:
                dependency_relative = dependency.relative_to(root)
            except ValueError:
                continue
            if (
                dependency_relative.parts
                and dependency_relative.parts[0] == "src"
                and dependency.suffix == ".hpp"
            ):
                targeted.add(dependency)
            elif (
                dependency_relative.parts
                and dependency_relative.parts[0] == "test"
                and dependency.suffix in {".cpp", ".hpp"}
            ):
                # Follow thin wrappers and test-only helper headers, but do
                # not count implementation details included by src headers.
                stack.append(dependency)
    return targeted


def untested_headers(root: Path) -> list[Path]:
    root = root.resolve()
    headers = {
        path.resolve()
        for path in (root / "src").rglob("*.hpp")
    }
    covered = headers_targeted_by_tests(root)
    return sorted(headers - covered)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="list source headers without a direct test target"
    )
    parser.add_argument("root", nargs="?", type=Path, default=Path("."))
    parser.add_argument(
        "--require-complete",
        action="store_true",
        help="return nonzero when at least one header has no direct test target",
    )
    args = parser.parse_args(argv)
    root = args.root.resolve()
    missing = untested_headers(root)
    for path in missing:
        print(path.relative_to(root).as_posix())
    print(
        f"untested header count: {len(missing)}",
        file=sys.stderr,
    )
    return int(args.require_complete and bool(missing))


if __name__ == "__main__":
    raise SystemExit(main())
