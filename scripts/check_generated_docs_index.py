#!/usr/bin/env python3
"""Validate standalone entries in a competitive-verifier generated index."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

import yaml


STANDALONE_PREFIX = "test/standalone/"
STANDALONE_CATEGORY = STANDALONE_PREFIX


class GeneratedIndexError(ValueError):
    pass


def _front_matter(path: Path) -> dict[str, Any]:
    lines = path.read_text(encoding="utf-8").splitlines(keepends=True)
    if not lines or lines[0].strip() != "---":
        raise GeneratedIndexError("index front matter opening delimiter is missing")
    for index in range(1, len(lines)):
        if lines[index].strip() == "---":
            data = yaml.safe_load("".join(lines[1:index]))
            if isinstance(data, dict):
                return data
            break
    raise GeneratedIndexError("index front matter is malformed")


def _expected_standalone_tests(plan_path: Path) -> set[str]:
    plan = json.loads(plan_path.read_text(encoding="utf-8"))
    files = plan.get("files") if isinstance(plan, dict) else None
    if not isinstance(files, dict):
        raise GeneratedIndexError("verification plan does not contain files")
    expected = {
        path
        for path in files
        if isinstance(path, str)
        and path.startswith(STANDALONE_PREFIX)
        and path.endswith(".test.cpp")
    }
    if not expected:
        raise GeneratedIndexError("verification plan contains no standalone tests")
    return expected


def validate_generated_index(index_path: Path, plan_path: Path) -> None:
    front_matter = _front_matter(index_path)
    data = front_matter.get("data")
    top = data.get("top") if isinstance(data, dict) else None
    if not isinstance(top, list):
        raise GeneratedIndexError("index does not contain data.top")

    entries = [
        entry
        for entry in top
        if isinstance(entry, dict) and entry.get("type") == "Verification Files"
    ]
    if len(entries) != 1:
        raise GeneratedIndexError("index must contain one Verification Files entry")
    categories = entries[0].get("categories")
    if not isinstance(categories, list):
        raise GeneratedIndexError("Verification Files does not contain categories")
    standalone_categories = [
        category
        for category in categories
        if isinstance(category, dict) and category.get("name") == STANDALONE_CATEGORY
    ]
    if len(standalone_categories) != 1:
        raise GeneratedIndexError("test/standalone category is missing or duplicated")

    pages = standalone_categories[0].get("pages")
    if not isinstance(pages, list):
        raise GeneratedIndexError("test/standalone category does not contain pages")
    actual = {
        page.get("path")
        for page in pages
        if isinstance(page, dict) and isinstance(page.get("path"), str)
    }
    expected = _expected_standalone_tests(plan_path)
    missing = sorted(expected - actual)
    if missing:
        raise GeneratedIndexError(
            "standalone tests are missing from Verification Files: " + ", ".join(missing)
        )

    missing_targets = [
        path for path in sorted(expected) if not (index_path.parent / f"{path}.md").is_file()
    ]
    if missing_targets:
        raise GeneratedIndexError(
            "standalone index link targets are missing: " + ", ".join(missing_targets)
        )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("index", type=Path)
    parser.add_argument("verification_plan", type=Path)
    args = parser.parse_args()
    try:
        validate_generated_index(args.index, args.verification_plan)
    except (OSError, UnicodeError, json.JSONDecodeError, GeneratedIndexError) as error:
        parser.error(str(error))
    print("generated docs index check passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
