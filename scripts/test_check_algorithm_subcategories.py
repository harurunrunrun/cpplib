#!/usr/bin/env python3

from pathlib import Path
from tempfile import TemporaryDirectory

import check_algorithm_subcategories


def main() -> None:
    assert check_algorithm_subcategories.EXPECTED_LAYOUT[
        Path("src/algorithm/fft")
    ] == frozenset({"formal_power_series", "min_plus", "transform"})
    assert check_algorithm_subcategories.EXPECTED_LAYOUT[
        Path("src/algorithm/range")
    ] == frozenset({
        "interval", "offline_query", "sliding_window", "static_query",
    })
    assert Path(
        "src/algorithm/sequence"
    ) in check_algorithm_subcategories.FORBIDDEN_ALGORITHM_ROOTS
    number_theory_root = Path("src/algorithm/math/number_theory")
    assert check_algorithm_subcategories.NESTED_LAYOUTS[number_theory_root] == (
        frozenset({
            "arithmetic", "gaussian", "modular", "multiplicative", "prime",
        })
    )
    assert len(
        check_algorithm_subcategories.NUMBER_THEORY_SUBCATEGORY_BY_STEM
    ) == 42
    assert (
        check_algorithm_subcategories.NUMBER_THEORY_SUBCATEGORY_BY_STEM[
            "gaussian_integer"
        ]
        == "gaussian"
    )
    shortest_path_root = Path("src/algorithm/graph/shortest_path")
    assert check_algorithm_subcategories.NESTED_LAYOUTS[shortest_path_root] == (
        frozenset({
            "all_pairs", "enumeration", "source", "specialized",
            "state_space",
        })
    )
    assert len(
        check_algorithm_subcategories.SHORTEST_PATH_SUBCATEGORY_BY_STEM
    ) == 25
    assert (
        check_algorithm_subcategories.SHORTEST_PATH_SUBCATEGORY_BY_STEM[
            "dijkstra"
        ]
        == "source"
    )

    connectivity_root = Path("src/algorithm/graph/connectivity")
    assert check_algorithm_subcategories.NESTED_LAYOUTS[connectivity_root] == (
        frozenset({
            "components", "decomposition", "resilience",
            "strongly_connected",
        })
    )
    assert len(
        check_algorithm_subcategories.CONNECTIVITY_SUBCATEGORY_BY_STEM
    ) == 22
    assert (
        check_algorithm_subcategories.CONNECTIVITY_SUBCATEGORY_BY_STEM[
            "lowlink"
        ]
        == "decomposition"
    )

    linear_algebra_root = Path("src/algorithm/math/linear_algebra")
    assert check_algorithm_subcategories.NESTED_LAYOUTS[
        linear_algebra_root
    ] == frozenset({
        "combinatorial", "elimination", "gf2", "matrix_operations",
    })
    assert len(
        check_algorithm_subcategories.LINEAR_ALGEBRA_SUBCATEGORY_BY_STEM
    ) == 21
    assert (
        check_algorithm_subcategories.LINEAR_ALGEBRA_SUBCATEGORY_BY_STEM[
            "fast_characteristic_polynomial"
        ]
        == "elimination"
    )

    dynamic_programming_root = Path(
        "src/algorithm/other/dynamic_programming"
    )
    assert check_algorithm_subcategories.NESTED_LAYOUTS[
        dynamic_programming_root
    ] == frozenset({
        "assignment_subset", "interval_partition", "profile_grid",
        "resource", "sequence_selection",
    })
    assert len(
        check_algorithm_subcategories.DYNAMIC_PROGRAMMING_SUBCATEGORY_BY_STEM
    ) == 22
    assert (
        check_algorithm_subcategories.DYNAMIC_PROGRAMMING_SUBCATEGORY_BY_STEM[
            "count_perfect_assignments"
        ]
        == "assignment_subset"
    )

    graph_specialized_root = Path("src/algorithm/graph/specialized")
    assert check_algorithm_subcategories.NESTED_LAYOUTS[
        graph_specialized_root
    ] == frozenset({
        "construction", "metric_analysis", "route_optimization",
        "structural_analysis", "subgraph_analysis",
    })
    assert len(
        check_algorithm_subcategories.GRAPH_SPECIALIZED_SUBCATEGORY_BY_STEM
    ) == 20
    assert (
        check_algorithm_subcategories.GRAPH_SPECIALIZED_SUBCATEGORY_BY_STEM[
            "treewidth_two_distance_oracle"
        ]
        == "structural_analysis"
    )

    graph_constraints_root = Path("src/algorithm/graph/constraints")
    assert check_algorithm_subcategories.NESTED_LAYOUTS[
        graph_constraints_root
    ] == frozenset({
        "coloring", "logical", "recognition", "selection", "systems",
    })
    assert len(
        check_algorithm_subcategories.GRAPH_CONSTRAINTS_SUBCATEGORY_BY_STEM
    ) == 13
    assert (
        check_algorithm_subcategories.GRAPH_CONSTRAINTS_SUBCATEGORY_BY_STEM[
            "difference_constraints"
        ]
        == "systems"
    )

    offline_query_root = Path("src/algorithm/other/offline_query")
    assert check_algorithm_subcategories.NESTED_LAYOUTS[
        offline_query_root
    ] == frozenset({
        "batch_framework", "grid_aggregation", "point_rectangle",
        "range_count",
    })
    assert len(
        check_algorithm_subcategories.OFFLINE_QUERY_SUBCATEGORY_BY_STEM
    ) == 13
    assert (
        check_algorithm_subcategories.OFFLINE_QUERY_SUBCATEGORY_BY_STEM[
            "parallel_binary_search"
        ]
        == "batch_framework"
    )


    with TemporaryDirectory() as directory:
        root = Path(directory)
        layout = {
            Path("src/algorithm/sample"): frozenset({"first", "second"}),
        }
        first = root / "src/algorithm/sample/first/value.hpp"
        second = root / "src/algorithm/sample/second/value.hpp"
        first.parent.mkdir(parents=True)
        second.parent.mkdir(parents=True)
        first.write_text("#ifndef FIRST\n#define FIRST\n#endif\n")
        second.write_text("#ifndef SECOND\n#define SECOND\n#endif\n")
        assert check_algorithm_subcategories.layout_violations(
            root, layout
        ) == []

        forbidden_root = Path("src/algorithm/legacy_sequence")
        forbidden = root / forbidden_root / "value.hpp"
        forbidden.parent.mkdir(parents=True)
        forbidden.write_text("#ifndef FORBIDDEN\n#define FORBIDDEN\n#endif\n")
        errors = check_algorithm_subcategories.layout_violations(
            root,
            layout,
            frozenset(),
            frozenset({forbidden_root}),
        )
        assert any("forbidden top-level" in error for error in errors)
        forbidden.unlink()
        forbidden.parent.rmdir()

        flat = root / "src/algorithm/sample/flat.hpp"
        flat.write_text("#ifndef FLAT\n#define FLAT\n#endif\n")
        errors = check_algorithm_subcategories.layout_violations(root, layout)
        assert any("declared subcategory" in error for error in errors)
        flat.unlink()

        unknown = root / "src/algorithm/sample/third/value.hpp"
        unknown.parent.mkdir()
        unknown.write_text("#ifndef THIRD\n#define THIRD\n#endif\n")
        errors = check_algorithm_subcategories.layout_violations(root, layout)
        assert any("unknown subcategory" in error for error in errors)
        unknown.unlink()
        unknown.parent.rmdir()

        nested = root / "src/algorithm/sample/first/detail/value.hpp"
        nested.parent.mkdir()
        nested.write_text("#ifndef NESTED\n#define NESTED\n#endif\n")
        errors = check_algorithm_subcategories.layout_violations(root, layout)
        assert any("must not be nested" in error for error in errors)
        nested.unlink()
        nested.parent.rmdir()

        second.unlink()
        errors = check_algorithm_subcategories.layout_violations(root, layout)
        assert any("category has no header" in error for error in errors)

        flat_layouts = frozenset({
            Path("src/algorithm/geometry/2d"),
            Path("src/algorithm/geometry/3d"),
        })
        for relative, name in zip(flat_layouts, ("two.hpp", "three.hpp")):
            header = root / relative / name
            header.parent.mkdir(parents=True)
            header.write_text("#ifndef FLAT\n#define FLAT\n#endif\n")
        assert check_algorithm_subcategories.layout_violations(
            root, {}, flat_layouts
        ) == []
        nested_geometry = (
            root / "src/algorithm/geometry/2d/predicate/nested.hpp"
        )
        nested_geometry.parent.mkdir()
        nested_geometry.write_text(
            "#ifndef NESTED_GEOMETRY\n#define NESTED_GEOMETRY\n#endif\n"
        )
        errors = check_algorithm_subcategories.layout_violations(
            root, {}, flat_layouts
        )
        assert any("dimension directory" in error for error in errors)

        missing_layout = {
            Path("src/algorithm/missing"): frozenset({"category"}),
        }
        errors = check_algorithm_subcategories.layout_violations(
            root, missing_layout
        )
        assert any("directory is missing" in error for error in errors)

    with TemporaryDirectory() as directory:
        root = Path(directory)
        layout = {
            Path("src/algorithm/math"): frozenset({"number_theory"}),
        }
        nested_root = Path("src/algorithm/math/number_theory")
        nested_layouts = {
            nested_root: frozenset({"arithmetic", "prime"}),
        }
        expected_stems = {
            nested_root: {
                "basic_number_theory": "arithmetic",
                "prime_sieve": "prime",
            },
        }
        source_basic = root / nested_root / "arithmetic/basic_number_theory.hpp"
        source_prime = root / nested_root / "prime/prime_sieve.hpp"
        docs_root = Path("docs/algorithm/math/number_theory")
        doc_basic = root / docs_root / "arithmetic/basic_number_theory.md"
        doc_prime = root / docs_root / "prime/prime_sieve.md"
        for file in (source_basic, source_prime, doc_basic, doc_prime):
            file.parent.mkdir(parents=True, exist_ok=True)
            file.write_text("valid\n", encoding="utf-8")

        def nested_errors() -> list[str]:
            return check_algorithm_subcategories.layout_violations(
                root,
                layout,
                frozenset(),
                frozenset(),
                nested_layouts=nested_layouts,
                expected_nested_stems=expected_stems,
                check_nested_docs=True,
            )

        assert nested_errors() == []

        direct = root / nested_root / "basic_number_theory.hpp"
        source_basic.rename(direct)
        errors = nested_errors()
        assert any(
            "declared number_theory subcategory" in error
            for error in errors
        )
        direct.rename(source_basic)

        unknown = root / nested_root / "misc/extra.hpp"
        unknown.parent.mkdir()
        unknown.write_text("unknown\n", encoding="utf-8")
        assert any(
            "unknown number_theory subcategory" in error
            for error in nested_errors()
        )
        unknown.unlink()
        unknown.parent.rmdir()

        deeper = root / nested_root / "arithmetic/detail/extra.hpp"
        deeper.parent.mkdir()
        deeper.write_text("nested\n", encoding="utf-8")
        assert any(
            "must not be nested more finely" in error
            for error in nested_errors()
        )
        deeper.unlink()
        deeper.parent.rmdir()

        wrong = root / nested_root / "arithmetic/prime_sieve.hpp"
        source_prime.rename(wrong)
        assert any(
            "expected src/algorithm/math/number_theory/prime/prime_sieve.hpp"
            in error
            for error in nested_errors()
        )
        wrong.rename(source_prime)

        legacy = root / "test/standalone/legacy.test.cpp"
        legacy.parent.mkdir(parents=True)
        legacy.write_text(
            '#include "../../src/algorithm/math/number_theory/basic_number_theory.hpp"\n',
            encoding="utf-8",
        )
        assert any("legacy reference" in error for error in nested_errors())

    with TemporaryDirectory() as directory:
        root = Path(directory)
        graph_root = Path("src/algorithm/graph")
        connectivity_root = graph_root / "connectivity"
        categories = {
            "undirected_component_count": "components",
            "lowlink": "decomposition",
            "single_failure_connectivity": "resilience",
            "strongly_connected_components": "strongly_connected",
        }
        layout = {graph_root: frozenset({"connectivity"})}
        nested_layouts = {
            connectivity_root: frozenset(categories.values()),
        }
        expected_stems = {connectivity_root: categories}
        docs_root = Path("docs/algorithm/graph/connectivity")
        for stem, category in categories.items():
            source = root / connectivity_root / category / f"{stem}.hpp"
            document = root / docs_root / category / f"{stem}.md"
            source.parent.mkdir(parents=True, exist_ok=True)
            document.parent.mkdir(parents=True, exist_ok=True)
            source.write_text("valid\n", encoding="utf-8")
            document.write_text("valid\n", encoding="utf-8")

        def connectivity_errors() -> list[str]:
            return check_algorithm_subcategories.layout_violations(
                root,
                layout,
                frozenset(),
                frozenset(),
                nested_layouts=nested_layouts,
                expected_nested_stems=expected_stems,
                check_nested_docs=True,
            )

        assert connectivity_errors() == []
        legacy = root / "test/standalone/legacy_connectivity.test.cpp"
        legacy.parent.mkdir(parents=True)
        legacy.write_text(
            '#include "../../src/algorithm/graph/connectivity/lowlink.hpp"\n',
            encoding="utf-8",
        )
        assert any(
            "legacy reference" in error
            for error in connectivity_errors()
        )

    with TemporaryDirectory() as directory:
        root = Path(directory)
        math_root = Path("src/algorithm/math")
        linear_algebra_root = math_root / "linear_algebra"
        categories = {
            "hafnian": "combinatorial",
            "gaussian_elimination": "elimination",
            "gf2_matrix_rank": "gf2",
            "fast_matrix_multiply": "matrix_operations",
        }
        layout = {math_root: frozenset({"linear_algebra"})}
        nested_layouts = {
            linear_algebra_root: frozenset(categories.values()),
        }
        expected_stems = {linear_algebra_root: categories}
        docs_root = Path("docs/algorithm/math/linear_algebra")
        for stem, category in categories.items():
            source = root / linear_algebra_root / category / f"{stem}.hpp"
            document = root / docs_root / category / f"{stem}.md"
            source.parent.mkdir(parents=True, exist_ok=True)
            document.parent.mkdir(parents=True, exist_ok=True)
            source.write_text("valid\n", encoding="utf-8")
            document.write_text("valid\n", encoding="utf-8")

        def linear_algebra_errors() -> list[str]:
            return check_algorithm_subcategories.layout_violations(
                root,
                layout,
                frozenset(),
                frozenset(),
                nested_layouts=nested_layouts,
                expected_nested_stems=expected_stems,
                check_nested_docs=True,
            )

        assert linear_algebra_errors() == []
        legacy = root / "test/standalone/legacy_linear_algebra.test.cpp"
        legacy.parent.mkdir(parents=True)
        legacy.write_text(
            '#include "../../src/algorithm/math/linear_algebra/'
            'gaussian_elimination.hpp"\n',
            encoding="utf-8",
        )
        assert any(
            "legacy reference" in error
            for error in linear_algebra_errors()
        )

    print("algorithm subcategory checker tests passed")


if __name__ == "__main__":
    main()
