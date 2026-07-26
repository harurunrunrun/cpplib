#!/usr/bin/env python3
"""Validate the top-level organization of reusable data structures."""

from __future__ import annotations

import argparse
from pathlib import Path

ALLOWED_STRUCTURE_CATEGORIES = frozenset({
    "array",
    "bbst",
    "bit",
    "convex_hull_trick",
    "dsu",
    "fenwick_tree",
    "graph",
    "hash",
    "heap",
    "interval",
    "io",
    "matrix",
    "modint",
    "ordered_set",
    "persistence",
    "range_query",
    "segtree",
    "spatial",
    "string",
    "tree",
    "trie",
    "types",
    "wavelet_matrix",
})

NESTED_STRUCTURE_CATEGORIES = {
    "tree": frozenset({"centroid", "dynamic_forest", "query", "treap"}),
}

FLAT_STRUCTURE_CATEGORIES = frozenset({
    "bit",
    "fenwick_tree",
    "heap",
    "interval",
    "io",
    "ordered_set",
    "persistence",
    "range_query",
    "spatial",
    "types",
})

EXPECTED_STRUCTURE_CATEGORY = {
    "bitset_xor_shift": "bit",
    "dice": "types",
    "disjoint_sparse_table": "range_query",
    "double_ended_priority_queue": "heap",
    "dynamic_median_multiset": "ordered_set",
    "fastio": "io",
    "fenwick_tree": "fenwick_tree",
    "fenwick_tree_2d": "fenwick_tree",
    "kd_tree_2d": "spatial",
    "kinetic_tournament": "heap",
    "modulo_multiset_counter": "ordered_set",
    "monoid_merge_sort_tree": "range_query",
    "monotone_queue": "range_query",
    "noncommutative_prefix_product": "range_query",
    "online_rectangle_add_rectangle_sum": "spatial",
    "partially_persistent_storage": "persistence",
    "persistent_queue": "persistence",
    "persistent_queue_queries": "persistence",
    "point_add_rectangle_affine_rectangle_sum": "spatial",
    "prefix_sum": "range_query",
    "priority_sum_structure": "ordered_set",
    "range_add_point_get": "fenwick_tree",
    "range_assign_frequency": "interval",
    "range_priority_queue": "heap",
    "sparse_table": "range_query",
    "sqrt_tree": "range_query",
    "static_interval_coverage": "interval",
    "static_interval_maximum_overlap": "interval",
    "static_range_kth_smallest": "range_query",
    "static_range_maximum_frequency": "range_query",
    "static_range_minimum": "range_query",
    "static_range_product_less_equal_mod": "range_query",
    "static_range_sum_sqrt_tree": "range_query",
    "threshold_updated_indexed_multiset": "ordered_set",
    "xor_basis": "bit",
    "centroid_distance_index": "tree/centroid",
    "compressed_ordered_set": "ordered_set",
    "dynamic_forest_connectivity": "tree/dynamic_forest",
    "dynamic_forest_vertex_add_path_sum": "tree/dynamic_forest",
    "dynamic_forest_vertex_set_path_sum": "tree/dynamic_forest",
    "dynamic_marked_tree_diameter": "tree/centroid",
    "dynamic_monochromatic_tree": "tree/query",
    "dynamic_nearest_marked_vertex": "tree/centroid",
    "dynamic_tree_edge_path_sum": "tree/query",
    "dynamic_tree_vertex_path_sum": "tree/query",
    "dynamic_tree_vertex_subtree_sum": "tree/query",
    "implicit_treap": "tree/treap",
    "implicit_treap_deque": "tree/treap",
    "incremental_tree_centroid": "tree/centroid",
    "integer_set": "ordered_set",
    "lazy_link_cut_tree": "tree/dynamic_forest",
    "lazy_top_tree": "tree/dynamic_forest",
    "link_cut_tree": "tree/dynamic_forest",
    "marked_tree_diameter_queries": "tree/centroid",
    "monochromatic_component_max_tree": "tree/query",
    "monochromatic_component_size_tree": "tree/query",
    "nearest_marked_tree_queries": "tree/centroid",
    "root_path_first_marked": "tree/query",
    "subtree_add_sum_minimum_tree": "tree/query",
    "top_tree": "tree/dynamic_forest",
    "top_tree_internal": "tree/dynamic_forest",
    "treap": "tree/treap",
    "tree_edge_point_set_path_maximum": "tree/query",
    "tree_path_assign_max_subarray": "tree/query",
}

RECLASSIFIED_TREE_STEMS = frozenset({
    stem
    for stem, category in EXPECTED_STRUCTURE_CATEGORY.items()
    if category.startswith("tree/")
}) | frozenset({"compressed_ordered_set", "integer_set"})

ALGORITHM_EXCEPTIONS = {
    "incremental_interval_scheduling": Path(
        "src/algorithm/other/scheduling/incremental_interval_scheduling.hpp"
    ),
}

SCANNED_REFERENCE_SUFFIXES = frozenset({".cpp", ".hpp", ".md"})
LEGACY_STRUCTURE_REFERENCE = "structure/other/"
LEGACY_RELATIVE_STRUCTURE_REFERENCE = "../other/"


def relative(path: Path, root: Path) -> str:
    return path.relative_to(root).as_posix()


def public_files(directory: Path, suffix: str) -> list[Path]:
    if not directory.is_dir():
        return []
    return sorted(path for path in directory.rglob(f"*{suffix}") if path.is_file())


def check_layout(root: Path) -> list[str]:
    root = root.resolve()
    source_root = root / "src" / "structure"
    docs_root = root / "docs" / "structure"
    violations: list[str] = []

    for old_root, suffix in (
        (source_root / "other", ".hpp"),
        (docs_root / "other", ".md"),
    ):
        for path in public_files(old_root, suffix):
            violations.append(
                f"{relative(path, root)}: legacy structure/other category is forbidden"
            )

    for base, suffix, label in (
        (source_root, ".hpp", "header"),
        (docs_root, ".md", "documentation"),
    ):
        if not base.is_dir():
            continue
        for path in sorted(base.glob(f"*{suffix}")):
            violations.append(
                f"{relative(path, root)}: structure {label} must be in a category"
            )
        for category in sorted(path for path in base.iterdir() if path.is_dir()):
            files = public_files(category, suffix)
            if not files:
                continue
            if category.name not in ALLOWED_STRUCTURE_CATEGORIES:
                violations.append(
                    f"{relative(category, root)}: unknown structure category"
                )
                continue
            if category.name in FLAT_STRUCTURE_CATEGORIES:
                for path in files:
                    if path.parent != category:
                        violations.append(
                            f"{relative(path, root)}: {category.name} must not have subcategories"
                        )

    for family, allowed_subcategories in NESTED_STRUCTURE_CATEGORIES.items():
        for base, suffix, label in (
            (source_root, ".hpp", "header"),
            (docs_root, ".md", "documentation"),
        ):
            family_root = base / family
            if not family_root.is_dir():
                continue
            for path in sorted(family_root.glob(f"*{suffix}")):
                violations.append(
                    f"{relative(path, root)}: structure {label} must be in a {family} subcategory"
                )
            for subcategory in sorted(
                path for path in family_root.iterdir() if path.is_dir()
            ):
                files = public_files(subcategory, suffix)
                if not files:
                    continue
                if subcategory.name not in allowed_subcategories:
                    violations.append(
                        f"{relative(subcategory, root)}: unknown {family} subcategory"
                    )
                    continue
                for path in files:
                    if path.parent != subcategory:
                        violations.append(
                            f"{relative(path, root)}: {family}/{subcategory.name} must not have nested subcategories"
                        )

    for stem, category in EXPECTED_STRUCTURE_CATEGORY.items():
        expected_header = source_root / category / f"{stem}.hpp"
        for path in sorted((root / "src").rglob(f"{stem}.hpp")):
            if path != expected_header:
                violations.append(
                    f"{relative(path, root)}: expected {relative(expected_header, root)}"
                )
        expected_doc = docs_root / category / f"{stem}.md"
        for path in sorted((root / "docs").rglob(f"{stem}.md")):
            if path != expected_doc:
                violations.append(
                    f"{relative(path, root)}: expected {relative(expected_doc, root)}"
                )

    for stem, expected_header_relative in ALGORITHM_EXCEPTIONS.items():
        expected_header = root / expected_header_relative
        for path in sorted((root / "src").rglob(f"{stem}.hpp")):
            if path != expected_header:
                violations.append(
                    f"{relative(path, root)}: expected {expected_header_relative.as_posix()}"
                )
        expected_doc_relative = Path(
            expected_header_relative.as_posix()
            .replace("src/", "docs/", 1)
            .replace(".hpp", ".md")
        )
        expected_doc = root / expected_doc_relative
        for path in sorted((root / "docs").rglob(f"{stem}.md")):
            if path != expected_doc:
                violations.append(
                    f"{relative(path, root)}: expected {expected_doc_relative.as_posix()}"
                )

    for top in ("src", "docs", "test"):
        directory = root / top
        if not directory.is_dir():
            continue
        for path in sorted(item for item in directory.rglob("*") if item.is_file()):
            if path.suffix not in SCANNED_REFERENCE_SUFFIXES:
                continue
            try:
                text = path.read_text(encoding="utf-8")
            except UnicodeDecodeError:
                continue
            legacy_references = [LEGACY_STRUCTURE_REFERENCE]
            legacy_references.extend(
                f"structure/tree/{stem}.hpp"
                for stem in RECLASSIFIED_TREE_STEMS
            )
            if path.is_relative_to(source_root):
                legacy_references.append(LEGACY_RELATIVE_STRUCTURE_REFERENCE)
                legacy_references.extend(
                    f"../tree/{stem}.hpp"
                    for stem in RECLASSIFIED_TREE_STEMS
                )
            for legacy_reference in legacy_references:
                if legacy_reference in text:
                    violations.append(
                        f"{relative(path, root)}: legacy reference {legacy_reference!r}"
                    )

    return violations


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("root", nargs="?", type=Path, default=Path("."))
    args = parser.parse_args()
    violations = check_layout(args.root)
    if violations:
        for violation in violations:
            print(violation)
        print(f"structure layout check failed: {len(violations)} violation(s)")
        return 1
    print("structure layout check passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
