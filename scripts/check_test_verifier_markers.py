#!/usr/bin/env python3
"""Validate competitive-verifier directives on every test source."""

from __future__ import annotations

import re
import sys
from pathlib import Path


PROBLEM_DIRECTIVE = re.compile(
    r"^\s*//\s*competitive-verifier:\s*PROBLEM\s+https?://\S+\s*$",
    re.MULTILINE,
)
LEGACY_PROBLEM_DIRECTIVE = re.compile(
    r"^\s*#define\s+PROBLEM(?:\s|$)",
    re.MULTILINE,
)
STANDALONE_DIRECTIVE = "// competitive-verifier: STANDALONE"


def find_violations(root: Path) -> list[str]:
    violations: list[str] = []
    onlinejudge = sorted((root / "test" / "onlinejudge").glob("*.test.cpp"))
    standalone_directory = root / "test" / "standalone"
    standalone_cpp_sources = sorted(
        path for path in standalone_directory.glob("*.cpp") if path.is_file()
    )
    standalone = [
        path for path in standalone_cpp_sources if path.name.endswith(".test.cpp")
    ]

    if not onlinejudge:
        violations.append("test/onlinejudge: no .test.cpp files found")
    if not standalone:
        violations.append("test/standalone: no .test.cpp files found")

    for path in standalone_cpp_sources:
        if path.name.endswith(".test.cpp"):
            continue
        relative = path.relative_to(root).as_posix()
        violations.append(
            f"{relative}: C++ sources directly under test/standalone must be "
            ".test.cpp entrypoints; move helper sources to test/support"
        )

    for path in onlinejudge:
        relative = path.relative_to(root).as_posix()
        try:
            source = path.read_text(encoding="utf-8")
        except (OSError, UnicodeError) as error:
            violations.append(f"{relative}: could not read: {error}")
            continue

        directives = PROBLEM_DIRECTIVE.findall(source)
        if len(directives) != 1:
            violations.append(
                f"{relative}: expected exactly one comment PROBLEM directive, "
                f"found {len(directives)}"
            )
        if LEGACY_PROBLEM_DIRECTIVE.search(source):
            violations.append(f"{relative}: legacy #define PROBLEM is forbidden")

    for path in standalone:
        relative = path.relative_to(root).as_posix()
        try:
            first_lines = path.read_text(encoding="utf-8").splitlines()[:10]
        except (OSError, UnicodeError) as error:
            violations.append(f"{relative}: could not read: {error}")
            continue
        if STANDALONE_DIRECTIVE not in first_lines:
            violations.append(
                f"{relative}: STANDALONE directive is missing from the first 10 lines"
            )

    return violations


def main(argv: list[str] | None = None) -> int:
    args = sys.argv[1:] if argv is None else argv
    root = Path(args[0]) if args else Path(".")
    violations = find_violations(root)
    if violations:
        for violation in violations:
            print(violation, file=sys.stderr)
        print(
            f"test verifier marker check failed: {len(violations)} violation(s)",
            file=sys.stderr,
        )
        return 1

    onlinejudge_count = len(list((root / "test" / "onlinejudge").glob("*.test.cpp")))
    standalone_count = len(list((root / "test" / "standalone").glob("*.test.cpp")))
    print(
        "test verifier marker check passed: "
        f"{onlinejudge_count} onlinejudge, {standalone_count} standalone"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
