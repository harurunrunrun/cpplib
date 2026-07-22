#!/usr/bin/env python3

from __future__ import annotations

import sys
from pathlib import Path


EXPECTED_LAYOUT: dict[Path, frozenset[str]] = {
    Path("src/algorithm/string"): frozenset({
        "automata", "palindrome", "sequence", "suffix",
    }),
    Path("src/algorithm/tree"): frozenset({
        "construction", "decomposition", "dynamic_programming",
        "isomorphism", "query",
    }),
    Path("src/algorithm/math"): frozenset({
        "combinatorics", "integer", "linear_algebra", "number_theory",
        "optimization", "polynomial", "sequence",
    }),
    Path("src/algorithm/graph"): frozenset({
        "connectivity", "constraints", "flow_cut", "shortest_path",
        "spanning_tree", "specialized", "traversal",
    }),
    Path("src/algorithm/matching"): frozenset({
        "bipartite", "flow", "general",
    }),
    Path("src/algorithm/other"): frozenset({
        "dynamic_programming", "offline_query", "scheduling", "sequence",
        "utility",
    }),
    Path("src/algorithm/random"): frozenset({"crack", "engine"}),
}


def layout_violations(
    repository_root: Path,
    expected_layout: dict[Path, frozenset[str]] = EXPECTED_LAYOUT,
) -> list[str]:
    errors: list[str] = []
    for relative_root, expected_categories in expected_layout.items():
        root = repository_root / relative_root
        if not root.is_dir():
            errors.append(f"{relative_root}: classified directory is missing")
            continue

        headers = sorted(path for path in root.rglob("*.hpp") if path.is_file())
        present_categories: set[str] = set()
        for header in headers:
            relative = header.relative_to(root)
            if len(relative.parts) == 1:
                errors.append(
                    f"{header.relative_to(repository_root)}: header must be "
                    "placed in a declared subcategory"
                )
                continue
            category = relative.parts[0]
            present_categories.add(category)
            if category not in expected_categories:
                errors.append(
                    f"{header.relative_to(repository_root)}: unknown "
                    f"subcategory '{category}'"
                )
            if len(relative.parts) != 2:
                errors.append(
                    f"{header.relative_to(repository_root)}: subcategories "
                    "must not be nested more finely"
                )

        for category in sorted(expected_categories - present_categories):
            errors.append(f"{relative_root / category}: category has no header")
    return errors


def run(repository_root: Path) -> int:
    errors = layout_violations(repository_root)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        print(
            f"algorithm subcategory check failed: {len(errors)} violation(s)",
            file=sys.stderr,
        )
        return 1
    header_count = sum(
        1
        for relative_root in EXPECTED_LAYOUT
        for path in (repository_root / relative_root).rglob("*.hpp")
        if path.is_file()
    )
    print(f"algorithm subcategory check passed: {header_count} header(s)")
    return 0


def main() -> int:
    return run(Path.cwd())


if __name__ == "__main__":
    raise SystemExit(main())
