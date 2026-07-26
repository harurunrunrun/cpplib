#!/usr/bin/env python3

from __future__ import annotations

import sys
from pathlib import Path


FLAT_LAYOUTS: frozenset[Path] = frozenset({
    Path("src/algorithm/geometry/2d"),
    Path("src/algorithm/geometry/3d"),
})


FORBIDDEN_ALGORITHM_ROOTS: frozenset[Path] = frozenset({
    Path("src/algorithm/sequence"),
})


EXPECTED_LAYOUT: dict[Path, frozenset[str]] = {
    Path("src/algorithm/fft"): frozenset({
        "formal_power_series", "min_plus", "transform",
    }),
    Path("src/algorithm/range"): frozenset({
        "interval", "offline_query", "sliding_window", "static_query",
    }),
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


NUMBER_THEORY_SUBCATEGORY_BY_STEM: dict[str, str] = {
    "basic_number_theory": "arithmetic",
    "binary_gcd": "arithmetic",
    "constant_time_gcd": "arithmetic",
    "enumerate_quotient_values": "arithmetic",
    "floor_sum": "arithmetic",
    "gcd_small_huge_integer": "arithmetic",
    "kth_root_integer": "arithmetic",
    "min_of_mod_of_linear": "arithmetic",
    "range_lcm": "arithmetic",
    "discrete_log": "modular",
    "discrete_logarithm_fixed_mod": "modular",
    "factorial_mod": "modular",
    "modular_arithmetic": "modular",
    "pisano_period": "modular",
    "power_tower_last_digits": "modular",
    "sqrt_mod": "modular",
    "tetration_mod": "modular",
    "count_prime_alternating_digit_sum_numbers": "prime",
    "count_primes": "prime",
    "lenstra_ecm_factorization": "prime",
    "nth_three_prime_factors": "prime",
    "prime_counting": "prime",
    "prime_factorization": "prime",
    "prime_sieve": "prime",
    "sampled_primes": "prime",
    "segmented_sieve": "prime",
    "smallest_prime_factor_prefix_sum": "prime",
    "associative_sieve": "multiplicative",
    "counting_squarefrees": "multiplicative",
    "divisor_count_prefix_sum": "multiplicative",
    "euler_totient_value": "multiplicative",
    "lcm_sum": "multiplicative",
    "pairwise_gcd_sums": "multiplicative",
    "pairwise_lcm_sum": "multiplicative",
    "proper_divisor_sum": "multiplicative",
    "square_divisor_count_prefix_sum": "multiplicative",
    "squarefree_integer_count": "multiplicative",
    "sum_of_multiplicative_function_large": "multiplicative",
    "sum_of_totient_function": "multiplicative",
    "visible_lattice_points": "multiplicative",
    "gaussian_integer": "gaussian",
    "gaussian_integer_factorization": "gaussian",
}

NUMBER_THEORY_SUBCATEGORIES = frozenset(
    NUMBER_THEORY_SUBCATEGORY_BY_STEM.values()
)

SHORTEST_PATH_SUBCATEGORY_BY_STEM: dict[str, str] = {
    "bellman_ford": "source",
    "dijkstra": "source",
    "multi_source_shortest_path": "source",
    "spfa": "source",
    "zero_one_bfs": "source",
    "floyd_warshall": "all_pairs",
    "johnson_all_pairs_shortest_paths": "all_pairs",
    "sum_shortest_path_query_costs": "all_pairs",
    "almost_shortest_path": "enumeration",
    "kth_shortest_path": "enumeration",
    "kth_shortest_walk": "enumeration",
    "shortest_path_edge_usage": "enumeration",
    "complement_shortest_path": "state_space",
    "complement_shortest_path_count": "state_space",
    "maximum_probability_path": "state_space",
    "minimum_forward_or_jump_cost": "state_space",
    "shortest_path_with_mode_switches": "state_space",
    "shortest_path_with_optional_edge": "state_space",
    "coordinate_product_knight_distances": "specialized",
    "count_vertices_reaching_target_within": "specialized",
    "directed_shortest_path_route": "specialized",
    "minimum_grid_character_changes": "specialized",
    "named_shortest_paths": "specialized",
    "undirected_shortest_path_distance": "specialized",
    "weighted_grid_shortest_path": "specialized",
}

SHORTEST_PATH_SUBCATEGORIES = frozenset(
    SHORTEST_PATH_SUBCATEGORY_BY_STEM.values()
)

NESTED_LAYOUTS: dict[Path, frozenset[str]] = {
    Path("src/algorithm/math/number_theory"): NUMBER_THEORY_SUBCATEGORIES,
    Path("src/algorithm/graph/shortest_path"): SHORTEST_PATH_SUBCATEGORIES,
}

EXPECTED_NESTED_STEMS: dict[Path, dict[str, str]] = {
    Path("src/algorithm/math/number_theory"): (
        NUMBER_THEORY_SUBCATEGORY_BY_STEM
    ),
    Path("src/algorithm/graph/shortest_path"): (
        SHORTEST_PATH_SUBCATEGORY_BY_STEM
    ),
}


def layout_violations(
    repository_root: Path,
    expected_layout: dict[Path, frozenset[str]] = EXPECTED_LAYOUT,
    flat_layouts: frozenset[Path] | None = None,
    forbidden_roots: frozenset[Path] | None = None,
    nested_layouts: dict[Path, frozenset[str]] | None = None,
    expected_nested_stems: dict[Path, dict[str, str]] | None = None,
    check_nested_docs: bool | None = None,
) -> list[str]:
    repository_defaults = expected_layout is EXPECTED_LAYOUT
    if flat_layouts is None:
        flat_layouts = (
            FLAT_LAYOUTS if expected_layout is EXPECTED_LAYOUT else frozenset()
        )
    if forbidden_roots is None:
        forbidden_roots = (
            FORBIDDEN_ALGORITHM_ROOTS
            if repository_defaults
            else frozenset()
        )
    if nested_layouts is None:
        nested_layouts = NESTED_LAYOUTS if repository_defaults else {}
    if expected_nested_stems is None:
        expected_nested_stems = (
            EXPECTED_NESTED_STEMS if repository_defaults else {}
        )
    if check_nested_docs is None:
        check_nested_docs = repository_defaults
    errors: list[str] = []
    for relative_root in forbidden_roots:
        root = repository_root / relative_root
        for header in sorted(root.rglob("*.hpp")):
            if header.is_file():
                errors.append(
                    f"{header.relative_to(repository_root)}: forbidden "
                    "top-level algorithm category"
                )
    for relative_root in flat_layouts:
        root = repository_root / relative_root
        if not root.is_dir():
            errors.append(f"{relative_root}: flat directory is missing")
            continue
        headers = sorted(path for path in root.rglob("*.hpp") if path.is_file())
        if not headers:
            errors.append(f"{relative_root}: flat directory has no header")
        for header in headers:
            if len(header.relative_to(root).parts) != 1:
                errors.append(
                    f"{header.relative_to(repository_root)}: header must be "
                    "placed directly in the dimension directory"
                )
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
            nested_root = relative_root / category
            if len(relative.parts) != 2 and nested_root not in nested_layouts:
                errors.append(
                    f"{header.relative_to(repository_root)}: subcategories "
                    "must not be nested more finely"
                )

        for category in sorted(expected_categories - present_categories):
            errors.append(f"{relative_root / category}: category has no header")

    for relative_root, expected_categories in nested_layouts.items():
        family = relative_root.name
        trees = [(relative_root, ".hpp", "header")]
        if check_nested_docs:
            docs_root = Path(
                relative_root.as_posix().replace("src/", "docs/", 1)
            )
            trees.append((docs_root, ".md", "documentation"))
        for relative_tree, suffix, label in trees:
            root = repository_root / relative_tree
            if not root.is_dir():
                errors.append(f"{relative_tree}: classified directory is missing")
                continue
            files = sorted(
                path for path in root.rglob(f"*{suffix}") if path.is_file()
            )
            present_categories: set[str] = set()
            for file in files:
                relative = file.relative_to(root)
                if len(relative.parts) == 1:
                    errors.append(
                        f"{file.relative_to(repository_root)}: {label} must be "
                        f"placed in a declared {family} subcategory"
                    )
                    continue
                category = relative.parts[0]
                present_categories.add(category)
                if category not in expected_categories:
                    errors.append(
                        f"{file.relative_to(repository_root)}: unknown "
                        f"{family} subcategory '{category}'"
                    )
                if len(relative.parts) != 2:
                    errors.append(
                        f"{file.relative_to(repository_root)}: {family} "
                        "subcategories must not be nested more finely"
                    )
            for category in sorted(expected_categories - present_categories):
                errors.append(
                    f"{relative_tree / category}: category has no {label}"
                )

    for relative_root, expected_stems in expected_nested_stems.items():
        source_root = repository_root / relative_root
        docs_relative_root = Path(
            relative_root.as_posix().replace("src/", "docs/", 1)
        )
        docs_root = repository_root / docs_relative_root
        for stem, category in expected_stems.items():
            expected_header = source_root / category / f"{stem}.hpp"
            matches = sorted(source_root.rglob(f"{stem}.hpp"))
            if matches != [expected_header]:
                if not matches:
                    errors.append(
                        f"{expected_header.relative_to(repository_root)}: "
                        "expected header is missing"
                    )
                for match in matches:
                    if match != expected_header:
                        errors.append(
                            f"{match.relative_to(repository_root)}: expected "
                            f"{expected_header.relative_to(repository_root)}"
                        )
            if check_nested_docs:
                expected_doc = docs_root / category / f"{stem}.md"
                doc_matches = sorted(docs_root.rglob(f"{stem}.md"))
                if doc_matches != [expected_doc]:
                    if not doc_matches:
                        errors.append(
                            f"{expected_doc.relative_to(repository_root)}: "
                            "expected documentation is missing"
                        )
                    for match in doc_matches:
                        if match != expected_doc:
                            errors.append(
                                f"{match.relative_to(repository_root)}: expected "
                                f"{expected_doc.relative_to(repository_root)}"
                            )

    scanned_suffixes = frozenset({".cpp", ".hpp", ".md"})
    for top in ("src", "docs", "test"):
        scan_root = repository_root / top
        if not scan_root.is_dir():
            continue
        for file in sorted(
            path for path in scan_root.rglob("*") if path.is_file()
        ):
            if file.suffix not in scanned_suffixes:
                continue
            try:
                body = file.read_text(encoding="utf-8")
            except UnicodeDecodeError:
                continue
            for relative_root, expected_stems in expected_nested_stems.items():
                legacy_root = relative_root.as_posix().removeprefix("src/")
                for stem in expected_stems:
                    legacy_reference = f"{legacy_root}/{stem}.hpp"
                    if legacy_reference in body:
                        errors.append(
                            f"{file.relative_to(repository_root)}: legacy "
                            f"reference '{legacy_reference}'"
                        )

    return errors


def run(repository_root: Path) -> int:
    errors = layout_violations(repository_root)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        print(
            f"algorithm layout check failed: {len(errors)} violation(s)",
            file=sys.stderr,
        )
        return 1
    header_count = sum(
        1
        for relative_root in (*FLAT_LAYOUTS, *EXPECTED_LAYOUT)
        for path in (repository_root / relative_root).rglob("*.hpp")
        if path.is_file()
    )
    print(f"algorithm layout check passed: {header_count} header(s)")
    return 0


def main() -> int:
    return run(Path.cwd())


if __name__ == "__main__":
    raise SystemExit(main())
