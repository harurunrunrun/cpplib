#!/usr/bin/env python3
"""Reject obsolete Wavelet Matrix implementation strategies."""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence


CHECKED_SUFFIXES = frozenset({".h", ".hh", ".hpp", ".hxx", ".ipp", ".tpp"})

OBSOLETE_PATTERNS: tuple[tuple[re.Pattern[str], str], ...] = (
    (
        re.compile(r"\bBLOCK_SIZE\b"),
        "BLOCK_SIZE compatibility parameters are forbidden",
    ),
    (
        re.compile(r"\bpersistent_block_reference\b", re.IGNORECASE),
        "the persistent block-reference implementation is forbidden",
    ),
    (
        re.compile(r"\bblock_(?:index|sorted)\b"),
        "square-decomposition block state is forbidden",
    ),
)

RANDOMIZED_DYNAMIC_PATTERNS: tuple[tuple[re.Pattern[str], str], ...] = (
    (
        re.compile(r"\b(?:mt19937|mt19937_64|random_device)\b"),
        "dynamic Wavelet Matrix storage must not depend on randomized priorities",
    ),
    (
        re.compile(r"\btreap_priority\b|\bpriority\s*=\s*(?:random|rng)"),
        "dynamic Wavelet Matrix storage must not be a randomized treap",
    ),
)

PERSISTENT_INCLUDE = re.compile(
    r"^\s*#\s*include\s*[<\"][^\">]*(?:^|/)persistent/[^\">]+[\">]",
    re.MULTILINE,
)

FULL_PERSISTENT_SYMBOL = re.compile(
    r"\bPersistent(?:BTree|WaveletMatrix)"
)


@dataclass(frozen=True)
class Violation:
    path: Path
    line: int
    detail: str

    def format(self, root: Path) -> str:
        try:
            logical = self.path.relative_to(root.parent)
        except ValueError:
            logical = self.path
        return f"{logical.as_posix()}:{self.line}: {self.detail}"


def _headers(root: Path) -> Iterable[Path]:
    for path in sorted(root.rglob("*")):
        if path.is_file() and path.suffix.lower() in CHECKED_SUFFIXES:
            yield path


def _line_number(source: str, offset: int) -> int:
    return source.count("\n", 0, offset) + 1


def find_violations(root: Path) -> tuple[list[Violation], int]:
    root = root.resolve()
    if not root.is_dir():
        return [Violation(root, 1, "Wavelet Matrix source root does not exist")], 0

    violations: list[Violation] = []
    checked = 0
    for path in _headers(root):
        checked += 1
        relative_parts = path.relative_to(root).parts
        if "persistent_block_reference" in path.name.lower():
            violations.append(
                Violation(path, 1, "the persistent block-reference implementation is forbidden")
            )
        try:
            source = path.read_text(encoding="utf-8")
        except (OSError, UnicodeError) as error:
            violations.append(Violation(path, 1, f"could not read UTF-8 text: {error}"))
            continue

        for pattern, detail in OBSOLETE_PATTERNS:
            for match in pattern.finditer(source):
                violations.append(
                    Violation(path, _line_number(source, match.start()), detail)
                )

        for pattern, detail in RANDOMIZED_DYNAMIC_PATTERNS:
            for match in pattern.finditer(source):
                violations.append(
                    Violation(path, _line_number(source, match.start()), detail)
                )

        if relative_parts and relative_parts[0] == "partially_persistent":
            for match in FULL_PERSISTENT_SYMBOL.finditer(source):
                violations.append(
                    Violation(
                        path,
                        _line_number(source, match.start()),
                        "partially persistent structures must not use fully persistent implementations",
                    )
                )

            for match in PERSISTENT_INCLUDE.finditer(source):
                violations.append(
                    Violation(
                        path,
                        _line_number(source, match.start()),
                        "partially persistent structures must not include fully persistent wrappers",
                    )
                )

    return violations, checked


def parse_arguments(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="reject obsolete Wavelet Matrix storage strategies"
    )
    parser.add_argument(
        "root",
        nargs="?",
        type=Path,
        default=Path("src/structure/wavelet_matrix"),
    )
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    arguments = parse_arguments(argv)
    violations, checked = find_violations(arguments.root)
    if violations:
        for violation in violations:
            print(violation.format(arguments.root.resolve()), file=sys.stderr)
        print(
            f"Wavelet Matrix design check failed: {len(violations)} violation(s), "
            f"{checked} header(s)",
            file=sys.stderr,
        )
        return 1
    print(f"Wavelet Matrix design check passed: {checked} header(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
