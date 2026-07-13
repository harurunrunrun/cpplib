#!/usr/bin/env python3
"""Check that documentation titles use ``English Name (日本語名)``."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path


def is_japanese_character(character: str) -> bool:
    """Return whether *character* belongs to a Japanese writing-system block."""

    codepoint = ord(character)
    return (
        0x3005 <= codepoint <= 0x3007  # Ideographic iteration/number marks.
        or 0x303B == codepoint  # Vertical ideographic iteration mark.
        or 0x3040 <= codepoint <= 0x30FF  # Hiragana and Katakana.
        or 0x31F0 <= codepoint <= 0x31FF  # Katakana phonetic extensions.
        or 0x3400 <= codepoint <= 0x4DBF  # CJK extension A.
        or 0x4E00 <= codepoint <= 0x9FFF  # CJK unified ideographs.
        or 0xF900 <= codepoint <= 0xFAFF  # CJK compatibility ideographs.
        or 0xFF66 <= codepoint <= 0xFF9F  # Half-width Katakana.
        or 0x20000 <= codepoint <= 0x323AF  # CJK extensions B through H.
    )


def decode_scalar(value: str) -> str:
    """Decode the simple quoted scalars used by the documentation front matter."""

    value = value.strip()
    if len(value) < 2 or value[0] != value[-1] or value[0] not in "\"'":
        return value
    if value[0] == "'":
        return value[1:-1].replace("''", "'")

    # YAML double-quoted strings support more escapes than this repository needs.
    # These common escapes are sufficient without making the checker depend on
    # a YAML package.
    escapes = {
        "0": "\0",
        "a": "\a",
        "b": "\b",
        "t": "\t",
        "n": "\n",
        "v": "\v",
        "f": "\f",
        "r": "\r",
        "e": "\x1b",
        " ": " ",
        '"': '"',
        "/": "/",
        "\\": "\\",
    }
    decoded: list[str] = []
    index = 1
    end = len(value) - 1
    while index < end:
        if value[index] != "\\":
            decoded.append(value[index])
            index += 1
            continue
        index += 1
        if index >= end:
            decoded.append("\\")
            break
        decoded.append(escapes.get(value[index], "\\" + value[index]))
        index += 1
    return "".join(decoded)


def front_matter_titles(path: Path) -> tuple[list[str], list[str]]:
    """Return title values and front-matter structural errors for *path*."""

    try:
        lines = path.read_text(encoding="utf-8-sig").splitlines()
    except (OSError, UnicodeError) as error:
        return [], [f"cannot read as UTF-8: {error}"]

    if not lines or lines[0].strip() != "---":
        return [], ["front matter is missing"]

    closing = next(
        (index for index, line in enumerate(lines[1:], start=1) if line.strip() == "---"),
        None,
    )
    if closing is None:
        return [], ["front matter is not terminated"]

    titles: list[str] = []
    for line in lines[1:closing]:
        key, separator, value = line.partition(":")
        if separator and key == "title":
            titles.append(decode_scalar(value))
    return titles, []


def split_bilingual_title(title: str) -> tuple[str, str] | None:
    """Split the final balanced ASCII-parenthesized suffix from *title*."""

    if not title.endswith(")"):
        return None

    depth = 0
    opening = -1
    for index in range(len(title) - 1, -1, -1):
        character = title[index]
        if character == ")":
            depth += 1
        elif character == "(":
            depth -= 1
            if depth == 0:
                opening = index
                break
            if depth < 0:
                return None

    if opening <= 0 or title[opening - 1] != " ":
        return None
    return title[: opening - 1], title[opening + 1 : -1]


def validate_title(title: str) -> list[str]:
    """Return all violations found in one title value."""

    parts = split_bilingual_title(title)
    if parts is None:
        return ["title must end with an ASCII-parenthesized Japanese name"]

    english, japanese = parts
    errors: list[str] = []
    if not english:
        errors.append("English name is empty")
    elif english != english.strip():
        errors.append("English name has leading or trailing whitespace")
    if any(is_japanese_character(character) for character in english):
        errors.append("English name contains Japanese characters")

    if not japanese:
        errors.append("Japanese name is empty")
    elif japanese != japanese.strip():
        errors.append("Japanese name has leading or trailing whitespace")
    if japanese and not any(is_japanese_character(character) for character in japanese):
        errors.append("Japanese name contains no Japanese characters")
    return errors


def markdown_files(arguments: list[str]) -> list[Path]:
    """Expand file and directory arguments into a stable, unique file list."""

    paths = [Path(argument) for argument in arguments]
    if not paths:
        paths = [Path(__file__).resolve().parent.parent / "docs"]

    files: set[Path] = set()
    for path in paths:
        if path.is_dir():
            files.update(candidate for candidate in path.glob("*.md") if candidate.is_file())
        else:
            files.add(path)
    return sorted(files, key=lambda path: str(path))


def check_file(path: Path) -> list[str]:
    titles, errors = front_matter_titles(path)
    if errors:
        return errors
    if not titles:
        return ["title is missing from front matter"]
    if len(titles) > 1:
        return [f"front matter contains {len(titles)} title fields"]
    return validate_title(titles[0])


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="check docs titles for the 'English Name (日本語名)' format"
    )
    parser.add_argument(
        "paths",
        nargs="*",
        help="Markdown files or flat documentation directories (default: docs)",
    )
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    files = markdown_files(arguments.paths)
    violations = 0
    invalid_files = 0
    for path in files:
        errors = check_file(path)
        if errors:
            invalid_files += 1
        for error in errors:
            print(f"{path}: {error}", file=sys.stderr)
            violations += 1

    if violations:
        print(
            "docs title check failed: "
            f"{invalid_files} invalid file(s), {violations} violation(s), "
            f"{len(files)} file(s) checked",
            file=sys.stderr,
        )
        return 1

    print(f"docs title check passed: {len(files)} file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
