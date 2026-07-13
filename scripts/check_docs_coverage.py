#!/usr/bin/env python3
"""Check documentation coverage and minimum content for library headers."""

from __future__ import annotations

import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path


LIBRARY_SECTIONS = ("algorithm", "structure", "approximate")
FRONT_MATTER_FIELD = re.compile(r"^documentation_of:\s*(.*?)\s*$")
MARKDOWN_HEADING = re.compile(r"^#{1,6}\s+(.+?)\s*$")
CAUTION_WORDS = ("API契約", "前提", "例外")
REQUIRED_TOPICS = ("時間計算量",)


def library_headers(source: Path) -> set[Path]:
    """Return all documented library headers, resolved to absolute paths."""

    return {
        path.resolve()
        for section in LIBRARY_SECTIONS
        for path in (source / section).rglob("*.hpp")
        if path.is_file()
    }


def documentation_target(path: Path) -> tuple[Path | None, list[str]]:
    """Read the unique ``documentation_of`` target from one Markdown file."""

    try:
        lines = path.read_text(encoding="utf-8-sig").splitlines()
    except (OSError, UnicodeError) as error:
        return None, [f"cannot read as UTF-8: {error}"]

    if not lines or lines[0].strip() != "---":
        return None, ["front matter is missing"]
    closing = next(
        (index for index, line in enumerate(lines[1:], start=1) if line.strip() == "---"),
        None,
    )
    if closing is None:
        return None, ["front matter is not terminated"]

    values: list[str] = []
    for line in lines[1:closing]:
        match = FRONT_MATTER_FIELD.fullmatch(line)
        if match:
            value = match.group(1).strip()
            if len(value) >= 2 and value[0] == value[-1] and value[0] in "\"'":
                value = value[1:-1]
            values.append(value)

    if not values:
        return None, ["documentation_of is missing from front matter"]
    if len(values) != 1:
        return None, [f"front matter contains {len(values)} documentation_of fields"]
    if not values[0]:
        return None, ["documentation_of is empty"]
    return (path.parent / values[0]).resolve(), []


def documentation_content_errors(path: Path) -> list[str]:
    """Return missing required content and non-normalized caution headings."""

    try:
        lines = path.read_text(encoding="utf-8-sig").splitlines()
    except (OSError, UnicodeError):
        return []

    errors: list[str] = []
    for topic in REQUIRED_TOPICS:
        if not any(topic in line for line in lines):
            errors.append(f"required topic is missing: {topic}")

    has_caution_heading = False
    for line_number, line in enumerate(lines, start=1):
        match = MARKDOWN_HEADING.fullmatch(line)
        if not match:
            continue
        heading = match.group(1).strip()
        if heading == "注意点":
            has_caution_heading = True
        if heading != "注意点" and any(word in heading for word in CAUTION_WORDS):
            errors.append(
                f"line {line_number}: caution-like heading must be '注意点': {heading}"
            )
    if not has_caution_heading:
        errors.append("required heading is missing: 注意点")

    return errors

def display(path: Path, root: Path) -> str:

    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return str(path)


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="check docs coverage, required content, and caution headings"
    )
    parser.add_argument("source", nargs="?", type=Path, default=Path("src"))
    parser.add_argument("docs", nargs="?", type=Path, default=Path("docs"))
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    source = arguments.source.resolve()
    docs = arguments.docs.resolve()
    root = Path.cwd().resolve()
    headers = library_headers(source)
    mappings: dict[Path, list[Path]] = defaultdict(list)
    errors: list[str] = []

    if not source.is_dir():
        errors.append(f"{display(source, root)}: source directory does not exist")
    if not docs.is_dir():
        errors.append(f"{display(docs, root)}: docs directory does not exist")

    markdown_files = sorted(docs.glob("*.md"), key=lambda path: path.as_posix())
    for path in markdown_files:
        target, target_errors = documentation_target(path)
        errors.extend(f"{display(path, root)}: {error}" for error in target_errors)
        errors.extend(
            f"{display(path, root)}: {error}" for error in documentation_content_errors(path)
        )
        if target is not None:
            mappings[target].append(path.resolve())

    for header in sorted(headers - mappings.keys(), key=lambda path: path.as_posix()):
        errors.append(f"{display(header, root)}: documentation is missing")

    for target, paths in sorted(mappings.items(), key=lambda item: item[0].as_posix()):
        path_list = ", ".join(display(path, root) for path in paths)
        if target not in headers:
            errors.append(
                f"{path_list}: documentation_of target is not a library header: "
                f"{display(target, root)}"
            )
        if len(paths) > 1:
            errors.append(
                f"{display(target, root)}: documented by {len(paths)} files: {path_list}"
            )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        print(
            "docs coverage check failed: "
            f"{len(errors)} violation(s), {len(headers)} header(s), "
            f"{len(markdown_files)} doc file(s)",
            file=sys.stderr,
        )
        return 1

    print(
        "docs coverage check passed: "
        f"{len(headers)} header(s), {len(markdown_files)} doc file(s)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
