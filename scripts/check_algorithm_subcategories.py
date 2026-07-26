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

CONNECTIVITY_SUBCATEGORY_BY_STEM: dict[str, str] = {
    "undirected_component_count": "components",
    "sparse_undirected_component_count": "components",
    "connected_components_of_complement_graph": "components",
    "connected_component_value_sums": "components",
    "strongly_connected_components": "strongly_connected",
    "iterative_strongly_connected_components": "strongly_connected",
    "strong_component_count": "strongly_connected",
    "sink_scc_vertices": "strongly_connected",
    "unique_sink_scc_vertices": "strongly_connected",
    "incremental_scc": "strongly_connected",
    "incremental_scc_pair_product_sums": "strongly_connected",
    "lowlink": "decomposition",
    "articulation_vertex_count": "decomposition",
    "two_edge_connected_components": "decomposition",
    "two_vertex_connected_components": "decomposition",
    "three_edge_connected_components": "decomposition",
    "block_cut_tree": "decomposition",
    "st_numbering": "decomposition",
    "single_failure_connectivity": "resilience",
    "endpoint_failure_connectivity": "resilience",
    "dominator_tree": "resilience",
    "pairwise_disconnect_cost_sum": "resilience",
}

CONNECTIVITY_SUBCATEGORIES = frozenset(
    CONNECTIVITY_SUBCATEGORY_BY_STEM.values()
)

LINEAR_ALGEBRA_SUBCATEGORY_BY_STEM: dict[str, str] = {
    "fast_matrix_multiply": "matrix_operations",
    "fast_matrix_multiply_mod": "matrix_operations",
    "matrix_power_mod": "matrix_operations",
    "fast_matrix_power_mod": "matrix_operations",
    "matrix_power_entry_queries": "matrix_operations",
    "fast_matrix_power_entry_queries": "matrix_operations",
    "gaussian_elimination": "elimination",
    "fast_gaussian_elimination": "elimination",
    "characteristic_polynomial": "elimination",
    "fast_characteristic_polynomial": "elimination",
    "fast_determinant": "elimination",
    "adjugate_matrix": "elimination",
    "fast_adjugate_matrix": "elimination",
    "intersection_of_f2_vector_spaces": "gf2",
    "gf2_determinant": "gf2",
    "gf2_matrix_rank": "gf2",
    "inverse_matrix_mod_2": "gf2",
    "matrix_product_mod_2": "gf2",
    "quadratic_form_gf2": "gf2",
    "hafnian": "combinatorial",
    "pfaffian": "combinatorial",
}

LINEAR_ALGEBRA_SUBCATEGORIES = frozenset(
    LINEAR_ALGEBRA_SUBCATEGORY_BY_STEM.values()
)

DYNAMIC_PROGRAMMING_SUBCATEGORY_BY_STEM: dict[str, str] = {
    "minimum_weighted_left_endpoint_partition_cost": "interval_partition",
    "minimum_rectangle_batch_cost": "interval_partition",
    "maximum_time_weighted_endpoint_removal_score": "interval_partition",
    "parenthesized_expression_extrema": "interval_partition",
    "minimum_adjacent_modular_merge_cost": "interval_partition",
    "minimize_maximum_partition_sum": "interval_partition",
    "minimum_total_interval_cut_cost": "interval_partition",
    "maximum_quadratic_partition_score": "interval_partition",
    "knapsack_max_value_min_cost": "resource",
    "minimum_exact_weight_cost_with_item_limit": "resource",
    "minimum_cost_two_resource_cover": "resource",
    "minimum_cost_path_with_resource_limit": "resource",
    "count_three_row_domino_tilings": "profile_grid",
    "count_four_row_domino_tilings": "profile_grid",
    "maximum_downward_grid_path_sum": "profile_grid",
    "minimum_ordered_pair_assignment_cost": "assignment_subset",
    "count_perfect_assignments": "assignment_subset",
    "maximum_histogram_perimeter_permutations": "assignment_subset",
    "maximum_weight_nonoverlapping_intervals": "sequence_selection",
    "maximum_disjoint_equal_pair_counts": "sequence_selection",
    "maximum_noncrossing_bipartite_segments": "sequence_selection",
    "minimum_rank_removal_sort_cost": "sequence_selection",
}

DYNAMIC_PROGRAMMING_SUBCATEGORIES = frozenset(
    DYNAMIC_PROGRAMMING_SUBCATEGORY_BY_STEM.values()
)

GRAPH_SPECIALIZED_SUBCATEGORY_BY_STEM: dict[str, str] = {
    "counting_c4": "subgraph_analysis",
    "enumerate_triangles": "subgraph_analysis",
    "sorted_bridge_endpoints": "subgraph_analysis",
    "triangle_weight_product_sum_mod": "subgraph_analysis",
    "grid_forest_diameter": "metric_analysis",
    "irreducible_pairs_from_bounded_integer_distance_matrix": (
        "metric_analysis"
    ),
    "irreducible_pairs_from_distance_matrix": "metric_analysis",
    "irreducible_pairs_from_distance_matrix_subcubic": "metric_analysis",
    "irreducible_pairs_from_small_integer_distance_matrix_ntt": (
        "metric_analysis"
    ),
    "ordered_quota_euclidean_activation_times": "metric_analysis",
    "radius_balls_form_vertex_partition": "metric_analysis",
    "chinese_postman": "route_optimization",
    "maximum_distinct_vertex_weight_walk": "route_optimization",
    "minimum_steiner_tree": "route_optimization",
    "steiner_tree": "route_optimization",
    "planarity_test": "structural_analysis",
    "tree_decomposition_width_two": "structural_analysis",
    "treewidth_two_distance_oracle": "structural_analysis",
    "range_edge_graph": "construction",
    "rearrange_rows_into_permutation_columns": "construction",
}

GRAPH_SPECIALIZED_SUBCATEGORIES = frozenset(
    GRAPH_SPECIALIZED_SUBCATEGORY_BY_STEM.values()
)

GRAPH_CONSTRAINTS_SUBCATEGORY_BY_STEM: dict[str, str] = {
    "horn_sat": "logical",
    "three_sat": "logical",
    "two_sat": "logical",
    "two_sat_assignment": "logical",
    "chromatic_number": "coloring",
    "is_bipartite": "coloring",
    "bipartite_edge_coloring": "coloring",
    "maximum_clique": "selection",
    "maximum_independent_set": "selection",
    "difference_constraints": "systems",
    "count_inconsistent_modulo_three_constraints": "systems",
    "chordal_graph_recognition": "recognition",
    "can_order_strings_by_matching_endpoints": "recognition",
}

GRAPH_CONSTRAINTS_SUBCATEGORIES = frozenset(
    GRAPH_CONSTRAINTS_SUBCATEGORY_BY_STEM.values()
)

NESTED_LAYOUTS: dict[Path, frozenset[str]] = {
    Path("src/algorithm/math/number_theory"): NUMBER_THEORY_SUBCATEGORIES,
    Path("src/algorithm/graph/shortest_path"): SHORTEST_PATH_SUBCATEGORIES,
    Path("src/algorithm/graph/connectivity"): CONNECTIVITY_SUBCATEGORIES,
    Path("src/algorithm/math/linear_algebra"): (
        LINEAR_ALGEBRA_SUBCATEGORIES
    ),
    Path("src/algorithm/other/dynamic_programming"): (
        DYNAMIC_PROGRAMMING_SUBCATEGORIES
    ),
    Path("src/algorithm/graph/specialized"): (
        GRAPH_SPECIALIZED_SUBCATEGORIES
    ),
    Path("src/algorithm/graph/constraints"): (
        GRAPH_CONSTRAINTS_SUBCATEGORIES
    ),
}

EXPECTED_NESTED_STEMS: dict[Path, dict[str, str]] = {
    Path("src/algorithm/math/number_theory"): (
        NUMBER_THEORY_SUBCATEGORY_BY_STEM
    ),
    Path("src/algorithm/graph/shortest_path"): (
        SHORTEST_PATH_SUBCATEGORY_BY_STEM
    ),
    Path("src/algorithm/graph/connectivity"): (
        CONNECTIVITY_SUBCATEGORY_BY_STEM
    ),
    Path("src/algorithm/math/linear_algebra"): (
        LINEAR_ALGEBRA_SUBCATEGORY_BY_STEM
    ),
    Path("src/algorithm/other/dynamic_programming"): (
        DYNAMIC_PROGRAMMING_SUBCATEGORY_BY_STEM
    ),
    Path("src/algorithm/graph/specialized"): (
        GRAPH_SPECIALIZED_SUBCATEGORY_BY_STEM
    ),
    Path("src/algorithm/graph/constraints"): (
        GRAPH_CONSTRAINTS_SUBCATEGORY_BY_STEM
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
