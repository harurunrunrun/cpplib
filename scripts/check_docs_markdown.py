#!/usr/bin/env python3
"""Check Markdown constructs that break competitive-verifier's rendered pages."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


BACKTICK = chr(96)
FENCE = re.compile(
    rf"^\s*([{re.escape(BACKTICK)}~]{{3,}})"
)
HEADING = re.compile(r"^(#{1,6})[ \t]+\S")
UNESCAPED_PIPE = re.compile(r"(?<!\\)\|")
RAW_TAG = re.compile(
    r"<\s*/?\s*([A-Za-z][A-Za-z0-9:_-]*)\b(?=[\s,/>])[^<>]*>"
)
MARKDOWN_LINK = re.compile(
    r"!?\[[^\]]*\]\(\s*([^\s)]+)(?:\s+['\"][^'\"]*['\"])?\s*\)"
)
ALLOWED_HTML_TAGS = {
    "a",
    "br",
    "code",
    "details",
    "div",
    "em",
    "img",
    "kbd",
    "li",
    "ol",
    "p",
    "pre",
    "span",
    "strong",
    "sub",
    "summary",
    "sup",
    "table",
    "tbody",
    "td",
    "th",
    "thead",
    "tr",
    "ul",
}
LIQUID_OPENERS = ("{{", "{%")


def strip_inline_code(line: str) -> tuple[str, bool]:
    """Remove inline-code spans and report whether every span was closed."""

    result: list[str] = []
    index = 0
    while True:
        opening = line.find(BACKTICK, index)
        if opening < 0:
            result.append(line[index:])
            return "".join(result), True

        result.append(line[index:opening])
        run_end = opening
        while run_end < len(line) and line[run_end] == BACKTICK:
            run_end += 1
        delimiter = BACKTICK * (run_end - opening)
        closing = line.find(delimiter, run_end)
        if closing < 0:
            result.append(line[opening:])
            return "".join(result), False
        index = closing + len(delimiter)


def is_table_row(line: str) -> bool:
    stripped = line.strip()
    return stripped.startswith("|") and stripped.endswith("|")


def table_errors(
    path: Path,
    rows: list[tuple[int, str]],
) -> list[str]:
    """Check one contiguous pipe-table block."""

    if len(rows) < 2 or "---" not in rows[1][1]:
        return []

    expected = len(UNESCAPED_PIPE.findall(rows[0][1]))
    errors: list[str] = []
    for line_number, line in rows[1:]:
        actual = len(UNESCAPED_PIPE.findall(line))
        if actual != expected:
            errors.append(
                f"{path}:{line_number}: Markdown table has {actual - 1} columns; "
                f"header has {expected - 1}. Escape literal pipes or use "
                r"\lvert and \rvert in formulas."
            )
    return errors


def check_file(path: Path) -> list[str]:
    """Return render-affecting Markdown errors in one documentation file."""

    try:
        lines = path.read_text(encoding="utf-8-sig").splitlines()
    except (OSError, UnicodeError) as error:
        return [f"{path}: cannot read as UTF-8: {error}"]

    errors: list[str] = []
    table_rows: list[tuple[int, str]] = []
    fence_character: str | None = None
    fence_length = 0
    fence_line = 0

    body_start_index = 0
    if lines and lines[0].strip() == "---":
        for index, line in enumerate(lines[1:], start=1):
            if line.strip() == "---":
                body_start_index = index + 1
                break

    def flush_table() -> None:
        nonlocal table_rows
        errors.extend(table_errors(path, table_rows))
        table_rows = []

    for line_index, line in enumerate(lines):
        line_number = line_index + 1

        # Jekyll renders Liquid before Markdown, including fenced and inline
        # code. Documentation in this repository does not intentionally use
        # Liquid, so reject every opener before the fence handling below can
        # skip code contents.
        if line_index >= body_start_index:
            for opener in LIQUID_OPENERS:
                if opener in line:
                    errors.append(
                        f"{path}:{line_number}: Liquid opener {opener!r} is not "
                        "allowed; Jekyll evaluates it even inside Markdown code"
                    )

        fence = FENCE.match(line)
        if fence:
            flush_table()
            delimiter = fence.group(1)
            if fence_character is None:
                fence_character = delimiter[0]
                fence_length = len(delimiter)
                fence_line = line_number
            elif delimiter[0] == fence_character and len(delimiter) >= fence_length:
                fence_character = None
                fence_length = 0
                fence_line = 0
            continue

        if fence_character is not None:
            continue

        if is_table_row(line):
            table_rows.append((line_number, line))
        else:
            flush_table()

        if line.startswith("    "):
            continue

        heading = HEADING.match(line)
        if line_index >= body_start_index and heading:
            if len(heading.group(1)) == 1:
                errors.append(
                    f"{path}:{line_number}: body heading must start at level 2; "
                    "the generated page already supplies its level-1 title"
                )
            if line_index > 0 and lines[line_index - 1].strip():
                errors.append(
                    f"{path}:{line_number}: heading must have a blank line before it"
                )
            if line_index + 1 < len(lines) and lines[line_index + 1].strip():
                errors.append(
                    f"{path}:{line_number}: heading must have a blank line after it"
                )

        prose, inline_balanced = strip_inline_code(line)
        if not inline_balanced:
            errors.append(f"{path}:{line_number}: inline-code span is not closed")

        for tag in RAW_TAG.findall(prose):
            if tag.lower() not in ALLOWED_HTML_TAGS:
                errors.append(
                    f"{path}:{line_number}: raw HTML-like <{tag}> hides template "
                    "text in the generated page; wrap the expression in inline code"
                )

        for target in MARKDOWN_LINK.findall(prose):
            target_path = target.split("#", 1)[0].split("?", 1)[0]
            if (
                target_path.lower().endswith(".md")
                and not target.startswith(("http://", "https://", "mailto:"))
            ):
                errors.append(
                    f"{path}:{line_number}: relative Markdown link {target!r} "
                    "is broken after documentation is embedded in a header page"
                )

    flush_table()
    if fence_character is not None:
        errors.append(f"{path}:{fence_line}: fenced code block is not closed")
    return errors


def markdown_files(paths: list[Path]) -> list[Path]:
    files: set[Path] = set()
    for path in paths:
        if path.is_dir():
            files.update(candidate for candidate in path.rglob("*.md") if candidate.is_file())
        else:
            files.add(path)
    return sorted(files, key=lambda path: path.as_posix())


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="check Markdown constructs that break rendered library docs"
    )
    parser.add_argument("paths", nargs="*", type=Path, default=[Path("docs")])
    return parser.parse_args()


def main() -> int:
    files = markdown_files(parse_arguments().paths)
    errors = [error for path in files for error in check_file(path)]
    if errors:
        print(*errors, sep="\n", file=sys.stderr)
        print(
            f"docs Markdown check failed: {len(errors)} violation(s), "
            f"{len(files)} file(s)",
            file=sys.stderr,
        )
        return 1
    print(f"docs Markdown check passed: {len(files)} file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
