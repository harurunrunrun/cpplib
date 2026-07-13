#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Any

import yaml


SECTION_ORDER = ("algorithm", "structure", "approximate")
VERIFICATION_PATH_PREFIX = "test/onlinejudge/"


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


def section_name(category: Any) -> str | None:
    if not isinstance(category, dict):
        return None
    name = category.get("name")
    if not isinstance(name, str):
        return None
    components = name.strip("/").split("/")
    if len(components) < 2 or components[0] != "src":
        return None
    return components[1]


def page_path(page: Any) -> str | None:
    if not isinstance(page, dict):
        return None
    path = page.get("path")
    return path if isinstance(path, str) else None


def entry_categories(entry: dict[str, Any]) -> list[Any]:
    categories = entry.get("categories")
    if not isinstance(categories, list):
        raise ValueError(f"{entry.get('type', 'index entry')} does not contain categories")
    return categories


def category_pages(category: dict[str, Any]) -> list[Any]:
    pages = category.get("pages")
    if not isinstance(pages, list):
        raise ValueError("index category does not contain pages")
    return pages


def find_entry(top: list[Any], entry_type: str) -> dict[str, Any]:
    matches = [
        entry
        for entry in top
        if isinstance(entry, dict) and entry.get("type") == entry_type
    ]
    if len(matches) != 1:
        raise ValueError(f"exactly one {entry_type} entry is required")
    return matches[0]


def merged_pages(existing: list[Any], moved: list[Any]) -> list[Any]:
    by_path: dict[str, Any] = {}
    without_path: list[Any] = []
    for page in existing + moved:
        path = page_path(page)
        if path is None:
            without_path.append(page)
        elif path not in by_path:
            by_path[path] = page
    return [by_path[path] for path in sorted(by_path)] + without_path


def move_verification_pages(
    library_entry: dict[str, Any],
    verification_entry: dict[str, Any],
) -> bool:
    library_categories = entry_categories(library_entry)
    verification_categories = entry_categories(verification_entry)
    verification_by_name = {
        category.get("name"): category
        for category in verification_categories
        if isinstance(category, dict) and isinstance(category.get("name"), str)
    }

    changed = False
    kept_categories: list[Any] = []
    for category in library_categories:
        if not isinstance(category, dict):
            kept_categories.append(category)
            continue

        kept_pages: list[Any] = []
        moved_pages: list[Any] = []
        for page in category_pages(category):
            path = page_path(page)
            if path is not None and path.startswith(VERIFICATION_PATH_PREFIX):
                moved_pages.append(page)
            else:
                kept_pages.append(page)

        if not moved_pages:
            kept_categories.append(category)
            continue

        changed = True
        if kept_pages:
            category["pages"] = kept_pages
            kept_categories.append(category)

        moved_category = dict(category)
        moved_category["pages"] = merged_pages([], moved_pages)
        name = moved_category.get("name")
        destination = verification_by_name.get(name)
        if destination is None:
            verification_categories.append(moved_category)
            if isinstance(name, str):
                verification_by_name[name] = moved_category
        else:
            destination["pages"] = merged_pages(
                category_pages(destination), moved_pages
            )

    if changed:
        library_entry["categories"] = kept_categories
    return changed


def normalize_index(front_matter: dict[str, Any]) -> bool:
    data = front_matter.get("data")
    if not isinstance(data, dict):
        raise ValueError("front matter does not contain data")
    top = data.get("top")
    if not isinstance(top, list):
        raise ValueError("front matter data does not contain top")

    library_entry = find_entry(top, "Library Files")
    verification_entry = find_entry(top, "Verification Files")
    changed = move_verification_pages(library_entry, verification_entry)

    categories = entry_categories(library_entry)
    rank = {name: index for index, name in enumerate(SECTION_ORDER)}
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
        library_entry["categories"] = reordered
        changed = True

    library_position = top.index(library_entry)
    verification_position = top.index(verification_entry)
    if verification_position < library_position:
        top.pop(library_position)
        verification_position = top.index(verification_entry)
        top.insert(verification_position, library_entry)
        changed = True
    return changed


def validate_index(front_matter: dict[str, Any]) -> None:
    data = front_matter.get("data")
    top = data.get("top") if isinstance(data, dict) else None
    if not isinstance(top, list):
        raise ValueError("front matter data does not contain top")

    library_entry = find_entry(top, "Library Files")
    verification_entry = find_entry(top, "Verification Files")
    if top.index(library_entry) >= top.index(verification_entry):
        raise ValueError("Verification Files must follow Library Files")

    rank = {name: index for index, name in enumerate(SECTION_ORDER)}
    previous_rank = -1
    for category in entry_categories(library_entry):
        current_rank = rank.get(section_name(category), len(rank))
        if current_rank < previous_rank:
            raise ValueError("Library Files categories are out of order")
        previous_rank = current_rank

    for entry in top:
        if entry is verification_entry or not isinstance(entry, dict):
            continue
        categories = entry.get("categories")
        if categories is None:
            continue
        if not isinstance(categories, list):
            raise ValueError(f"{entry.get('type', 'index entry')} does not contain categories")
        for category in categories:
            if not isinstance(category, dict):
                continue
            for page in category_pages(category):
                path = page_path(page)
                if path is not None and path.startswith(VERIFICATION_PATH_PREFIX):
                    raise ValueError(
                        f"verification file is outside Verification Files: {path}"
                    )


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
            "Order competitive-verifier library categories and keep online-judge "
            "tests under Verification Files."
        )
    )
    parser.add_argument("index", type=Path)
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail instead of rewriting when the index is not normalized",
    )
    args = parser.parse_args()

    text = args.index.read_text(encoding="utf-8")
    front_matter, content = split_front_matter(text)
    changed = normalize_index(front_matter)
    validate_index(front_matter)
    if args.check:
        if changed:
            parser.error("competitive-verifier index is not normalized")
        return

    if changed:
        args.index.write_text(
            dump_front_matter(front_matter, content),
            encoding="utf-8",
        )


if __name__ == "__main__":
    main()
