#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Any

import yaml


SECTION_ORDER = ("algorithm", "structure", "approximate")


def split_front_matter(text: str) -> tuple[dict[str, Any], str]:
    lines = text.splitlines(keepends=True)
    if not lines or lines[0].strip() != "---":
        raise ValueError("front matter opening delimiter is missing")

    for index in range(1, len(lines)):
        if lines[index].strip() == "---":
            data = yaml.safe_load("".join(lines[1:index]))
            if not isinstance(data, dict):
                raise ValueError("front matter must be a mapping")
            return data, "".join(lines[index + 1 :])
    raise ValueError("front matter closing delimiter is missing")


def section_name(category: dict[str, Any]) -> str | None:
    name = category.get("name")
    if not isinstance(name, str):
        return None
    components = name.strip("/").split("/")
    if len(components) < 2 or components[0] != "src":
        return None
    return components[1]


def reorder_library_categories(front_matter: dict[str, Any]) -> bool:
    data = front_matter.get("data")
    if not isinstance(data, dict):
        raise ValueError("front matter does not contain data")
    top = data.get("top")
    if not isinstance(top, list):
        raise ValueError("front matter data does not contain top")

    rank = {name: index for index, name in enumerate(SECTION_ORDER)}
    changed = False
    found_library_files = False
    for entry in top:
        if not isinstance(entry, dict) or entry.get("type") != "Library Files":
            continue
        found_library_files = True
        categories = entry.get("categories")
        if not isinstance(categories, list):
            raise ValueError("Library Files does not contain categories")

        original_positions = {
            id(category): index for index, category in enumerate(categories)
        }
        reordered = sorted(
            categories,
            key=lambda category: (
                rank.get(section_name(category), len(rank)),
                original_positions[id(category)],
            ),
        )
        if reordered != categories:
            entry["categories"] = reordered
            changed = True

    if not found_library_files:
        raise ValueError("Library Files entry is missing")
    return changed


def dump_front_matter(front_matter: dict[str, Any], content: str) -> str:
    body = yaml.safe_dump(
        front_matter,
        allow_unicode=True,
        sort_keys=False,
        width=10_000,
    )
    return f"---\n{body}---\n{content}"


def main() -> None:
    parser = argparse.ArgumentParser(
        description=(
            "Order competitive-verifier library categories as "
            "algorithm, structure, approximate."
        )
    )
    parser.add_argument("index", type=Path)
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail instead of rewriting when the categories are out of order",
    )
    args = parser.parse_args()

    text = args.index.read_text(encoding="utf-8")
    front_matter, content = split_front_matter(text)
    changed = reorder_library_categories(front_matter)
    if args.check:
        if changed:
            parser.error(
                "library categories are not ordered as algorithm, structure, approximate"
            )
        return

    if changed:
        args.index.write_text(
            dump_front_matter(front_matter, content),
            encoding="utf-8",
        )


if __name__ == "__main__":
    main()
