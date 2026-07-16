#!/usr/bin/env python3
"""Reject Boost dependencies from library, test, and documentation files."""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence


CHECKED_SUFFIXES = frozenset(
    {
        ".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx",
        ".inc", ".ipp", ".md", ".tpp",
    }
)

# Match the standalone identifier as well as conventional Boost macro/helper
# prefixes. Identifier boundaries keep ordinary words such as "boosted" valid.
BOOST_REFERENCE = re.compile(
    r"(?<![A-Za-z0-9_])boost(?:_[A-Za-z0-9_]+)?(?![A-Za-z0-9_])",
    re.IGNORECASE,
)
BOOST_PATH_COMPONENT = re.compile(
    r"(?:^|[/_.-])boost(?=$|[/_.-])",
    re.IGNORECASE,
)


@dataclass(frozen=True)
class Violation:
    path: Path
    logical_path: Path
    rule: str
    detail: str
    line: int | None = None
    column: int | None = None

    def format(self) -> str:
        location = self.logical_path.as_posix()
        if self.line is not None:
            location += f":{self.line}"
            if self.column is not None:
                location += f":{self.column}"
        return f"{location}: {self.detail}"


def _candidate_files(root: Path) -> Iterable[tuple[Path, Path]]:
    if root.is_file():
        if root.suffix.lower() in CHECKED_SUFFIXES:
            yield root, Path(root.name)
        return
    if not root.is_dir():
        return
    for path in sorted(root.rglob("*")):
        if path.is_file() and path.suffix.lower() in CHECKED_SUFFIXES:
            yield path, Path(root.name) / path.relative_to(root)


def find_violations(roots: Sequence[Path]) -> tuple[list[Violation], int]:
    """Return all violations and the number of unique files inspected."""

    violations: list[Violation] = []
    candidates: dict[Path, tuple[Path, Path]] = {}
    for root in roots:
        if not root.exists():
            violations.append(
                Violation(
                    path=root,
                    logical_path=root,
                    rule="missing-root",
                    detail="scan root does not exist",
                )
            )
            continue
        for path, logical_path in _candidate_files(root):
            candidates.setdefault(path.resolve(), (path, logical_path))

    for _, (path, logical_path) in sorted(
        candidates.items(), key=lambda item: item[1][1].as_posix()
    ):
        if BOOST_PATH_COMPONENT.search(logical_path.as_posix()) is not None:
            violations.append(
                Violation(
                    path=path,
                    logical_path=logical_path,
                    rule="boost-path",
                    detail="Boost-named source or documentation path is forbidden",
                )
            )
        try:
            source = path.read_text(encoding="utf-8")
        except (OSError, UnicodeError) as error:
            violations.append(
                Violation(
                    path=path,
                    logical_path=logical_path,
                    rule="read-error",
                    detail=f"could not read UTF-8 text: {error}",
                )
            )
            continue
        for line_number, line in enumerate(source.splitlines(), start=1):
            for match in BOOST_REFERENCE.finditer(line):
                violations.append(
                    Violation(
                        path=path,
                        logical_path=logical_path,
                        rule="boost-reference",
                        detail=(
                            "Boost dependency reference is forbidden: "
                            f"{match.group(0)!r}"
                        ),
                        line=line_number,
                        column=match.start() + 1,
                    )
                )
    return violations, len(candidates)


def parse_arguments(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="reject Boost dependencies from repository-owned files"
    )
    parser.add_argument(
        "roots",
        nargs="*",
        type=Path,
        default=[Path("src"), Path("test"), Path("docs")],
        help="roots to scan (default: src test docs)",
    )
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    arguments = parse_arguments(argv)
    violations, checked_count = find_violations(arguments.roots)
    if violations:
        print(*(violation.format() for violation in violations), sep="\n", file=sys.stderr)
        print(
            "Boost dependency check failed: "
            f"{len(violations)} violation(s), {checked_count} file(s)",
            file=sys.stderr,
        )
        return 1
    print(f"Boost dependency check passed: {checked_count} file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
