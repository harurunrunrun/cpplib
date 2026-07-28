#!/usr/bin/env python3
"""Tests for check_structure_layout.py."""

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from check_structure_layout import check_layout


class CheckStructureLayoutTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary_directory.cleanup)
        self.root = Path(self.temporary_directory.name)

    def write(self, relative_path: str, body: str = "") -> Path:
        path = self.root / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(body, encoding="utf-8")
        return path

    def messages(self) -> list[str]:
        return check_layout(self.root)

    def test_valid_moderate_layout(self) -> None:
        self.write("src/structure/range_query/aggregation/sparse_table.hpp")
        self.write("docs/structure/range_query/aggregation/sparse_table.md")
        self.write(
            "src/algorithm/other/scheduling/incremental_interval_scheduling.hpp"
        )
        self.write(
            "docs/algorithm/other/scheduling/incremental_interval_scheduling.md"
        )
        self.write("src/structure/segtree/basic/lazysegtree.hpp")
        self.write("src/structure/dsu/basic/dsu.hpp")
        self.write("docs/structure/dsu/basic/dsu.md")
        self.write("src/structure/tree/dynamic_forest/link_cut_tree.hpp")
        self.write("docs/structure/tree/dynamic_forest/link_cut_tree.md")
        self.write("src/structure/ordered_set/set/integer_set.hpp")
        self.write("docs/structure/ordered_set/set/integer_set.md")
        self.write("src/structure/types/monoid/monoid.hpp")
        self.write("docs/structure/types/monoid/monoid.md")
        self.write("src/structure/types/simulation/dice.hpp")
        self.write("docs/structure/types/simulation/dice.md")
        self.write("src/structure/types/rational/rational.hpp")
        self.write("docs/structure/types/rational/rational.md")
        self.write(
            "src/structure/convex_hull_trick/li_chao/dynamic_li_chao_tree.hpp"
        )
        self.write("docs/structure/convex_hull_trick/li_chao/dynamic_li_chao_tree.md")
        self.write("src/structure/bbst/map/red_black_tree.hpp")
        self.write("docs/structure/bbst/map/red_black_tree.md")
        self.write("src/structure/trie/binary/binary_trie.hpp")
        self.write("docs/structure/trie/binary/binary_trie.md")
        self.write("src/structure/heap/meldable/persistent_leftist_heap.hpp")
        self.write("docs/structure/heap/meldable/persistent_leftist_heap.md")
        self.write(
            "src/structure/graph/dynamic_connectivity/online_dynamic_connectivity.hpp"
        )
        self.write(
            "docs/structure/graph/dynamic_connectivity/online_dynamic_connectivity.md"
        )
        self.write(
            "src/algorithm/graph/shortest_path/specialized/coordinate_product_knight_distances.hpp"
        )
        self.write(
            "docs/algorithm/graph/shortest_path/specialized/coordinate_product_knight_distances.md"
        )
        self.assertEqual(self.messages(), [])

    def test_legacy_other_header_and_document_are_rejected(self) -> None:
        self.write("src/structure/other/sparse_table.hpp")
        self.write("docs/structure/other/sparse_table.md")
        messages = self.messages()
        self.assertTrue(any("legacy structure/other category" in item for item in messages))

    def test_unknown_category_is_rejected(self) -> None:
        self.write("src/structure/misc/example.hpp")
        self.assertTrue(any("unknown structure category" in item for item in self.messages()))

    def test_uncategorized_header_is_rejected(self) -> None:
        self.write("src/structure/example.hpp")
        self.assertTrue(any("must be in a category" in item for item in self.messages()))

    def test_nested_family_header_must_be_in_a_subcategory(self) -> None:
        self.write("src/structure/range_query/sparse_table.hpp")
        self.assertTrue(any(
            "range_query subcategory" in item for item in self.messages()
        ))

    def test_unknown_range_query_subcategory_is_rejected(self) -> None:
        self.write("src/structure/range_query/static/sparse_table.hpp")
        self.assertTrue(any(
            "unknown range_query subcategory" in item
            for item in self.messages()
        ))

    def test_range_query_subcategory_cannot_be_split_further(self) -> None:
        self.write("src/structure/range_query/aggregation/detail/example.hpp")
        self.assertTrue(any(
            "must not have nested subcategories" in item
            for item in self.messages()
        ))

    def test_ordered_set_header_must_be_in_a_subcategory(self) -> None:
        self.write("src/structure/ordered_set/integer_set.hpp")
        self.assertTrue(any(
            "ordered_set subcategory" in item for item in self.messages()
        ))

    def test_types_header_in_wrong_subcategory_is_rejected(self) -> None:
        self.write("src/structure/types/simulation/monoid.hpp")
        self.assertTrue(any(
            "expected src/structure/types/monoid" in item
            for item in self.messages()
        ))

    def test_known_header_in_wrong_category_is_rejected(self) -> None:
        self.write("src/structure/heap/sparse_table.hpp")
        self.assertTrue(any("expected src/structure/range_query/aggregation" in item for item in self.messages()))

    def test_dsu_header_must_be_in_a_subcategory(self) -> None:
        self.write("src/structure/dsu/dsu.hpp")
        self.assertTrue(any("dsu subcategory" in item for item in self.messages()))

    def test_unknown_dsu_subcategory_is_rejected(self) -> None:
        self.write("src/structure/dsu/misc/example.hpp")
        self.assertTrue(any("unknown dsu subcategory" in item for item in self.messages()))

    def test_dsu_subcategory_cannot_be_split_further(self) -> None:
        self.write("src/structure/dsu/basic/internal/example.hpp")
        self.assertTrue(any("must not have nested subcategories" in item for item in self.messages()))

    def test_dsu_header_in_wrong_subcategory_is_rejected(self) -> None:
        self.write("src/structure/dsu/rollback/dsu.hpp")
        self.assertTrue(any("expected src/structure/dsu/basic" in item for item in self.messages()))

    def test_bbst_header_must_be_in_a_subcategory(self) -> None:
        self.write("src/structure/bbst/red_black_tree.hpp")
        self.assertTrue(any("bbst subcategory" in item for item in self.messages()))

    def test_unknown_bbst_subcategory_is_rejected(self) -> None:
        self.write("src/structure/bbst/misc/example.hpp")
        self.assertTrue(any("unknown bbst subcategory" in item for item in self.messages()))

    def test_bbst_header_in_wrong_subcategory_is_rejected(self) -> None:
        self.write("src/structure/bbst/sequence/red_black_tree.hpp")
        self.assertTrue(any(
            "expected src/structure/bbst/map" in item
            for item in self.messages()
        ))

    def test_bbst_subcategory_cannot_be_split_further(self) -> None:
        self.write("src/structure/bbst/map/internal/example.hpp")
        self.assertTrue(any(
            "must not have nested subcategories" in item
            for item in self.messages()
        ))

    def test_heap_header_must_be_in_a_subcategory(self) -> None:
        self.write("src/structure/heap/persistent_leftist_heap.hpp")
        self.assertTrue(any("heap subcategory" in item for item in self.messages()))

    def test_unknown_heap_subcategory_is_rejected(self) -> None:
        self.write("src/structure/heap/misc/example.hpp")
        self.assertTrue(any("unknown heap subcategory" in item for item in self.messages()))

    def test_heap_header_in_wrong_subcategory_is_rejected(self) -> None:
        self.write("src/structure/heap/priority_queue/persistent_leftist_heap.hpp")
        self.assertTrue(any(
            "expected src/structure/heap/meldable" in item
            for item in self.messages()
        ))

    def test_heap_subcategory_cannot_be_split_further(self) -> None:
        self.write("src/structure/heap/meldable/internal/example.hpp")
        self.assertTrue(any(
            "must not have nested subcategories" in item
            for item in self.messages()
        ))

    def test_graph_header_must_be_in_a_subcategory(self) -> None:
        self.write("src/structure/graph/online_dynamic_connectivity.hpp")
        self.assertTrue(any(
            "graph subcategory" in item for item in self.messages()
        ))

    def test_unknown_graph_subcategory_is_rejected(self) -> None:
        self.write("src/structure/graph/misc/example.hpp")
        self.assertTrue(any(
            "unknown graph subcategory" in item for item in self.messages()
        ))

    def test_graph_header_in_wrong_subcategory_is_rejected(self) -> None:
        self.write(
            "src/structure/graph/misc/online_dynamic_connectivity.hpp"
        )
        self.assertTrue(any(
            "expected src/structure/graph/dynamic_connectivity" in item
            for item in self.messages()
        ))

    def test_graph_subcategory_cannot_be_split_further(self) -> None:
        self.write(
            "src/structure/graph/dynamic_connectivity/detail/example.hpp"
        )
        self.assertTrue(any(
            "must not have nested subcategories" in item
            for item in self.messages()
        ))

    def test_convex_hull_trick_header_must_be_in_a_subcategory(self) -> None:
        self.write("src/structure/convex_hull_trick/dynamic_li_chao_tree.hpp")
        self.assertTrue(any(
            "convex_hull_trick subcategory" in item for item in self.messages()
        ))

    def test_unknown_convex_hull_trick_subcategory_is_rejected(self) -> None:
        self.write("src/structure/convex_hull_trick/misc/example.hpp")
        self.assertTrue(any(
            "unknown convex_hull_trick subcategory" in item
            for item in self.messages()
        ))

    def test_convex_hull_trick_header_in_wrong_subcategory_is_rejected(self) -> None:
        self.write(
            "src/structure/convex_hull_trick/line_container/dynamic_li_chao_tree.hpp"
        )
        self.assertTrue(any(
            "expected src/structure/convex_hull_trick/li_chao" in item
            for item in self.messages()
        ))

    def test_convex_hull_trick_subcategory_cannot_be_split_further(self) -> None:
        self.write("src/structure/convex_hull_trick/li_chao/internal/example.hpp")
        self.assertTrue(any("must not have nested subcategories" in item for item in self.messages()))


    def test_trie_header_must_be_in_a_subcategory(self) -> None:
        self.write("src/structure/trie/trie.hpp")
        self.assertTrue(any("trie subcategory" in item for item in self.messages()))

    def test_unknown_trie_subcategory_is_rejected(self) -> None:
        self.write("src/structure/trie/misc/example.hpp")
        self.assertTrue(any("unknown trie subcategory" in item for item in self.messages()))

    def test_trie_header_in_wrong_subcategory_is_rejected(self) -> None:
        self.write("src/structure/trie/string/binary_trie.hpp")
        self.assertTrue(any(
            "expected src/structure/trie/binary" in item
            for item in self.messages()
        ))

    def test_trie_subcategory_cannot_be_split_further(self) -> None:
        self.write("src/structure/trie/binary/internal/example.hpp")
        self.assertTrue(any(
            "must not have nested subcategories" in item
            for item in self.messages()
        ))

    def test_tree_header_must_be_in_a_subcategory(self) -> None:
        self.write("src/structure/tree/link_cut_tree.hpp")
        self.assertTrue(any("tree subcategory" in item for item in self.messages()))

    def test_unknown_tree_subcategory_is_rejected(self) -> None:
        self.write("src/structure/tree/misc/example.hpp")
        self.assertTrue(any("unknown tree subcategory" in item for item in self.messages()))

    def test_tree_subcategory_cannot_be_split_further(self) -> None:
        self.write("src/structure/tree/query/static/example.hpp")
        self.assertTrue(any("must not have nested subcategories" in item for item in self.messages()))

    def test_tree_header_in_wrong_subcategory_is_rejected(self) -> None:
        self.write("src/structure/tree/query/link_cut_tree.hpp")
        self.assertTrue(any("expected src/structure/tree/dynamic_forest" in item for item in self.messages()))

    def test_algorithm_exception_in_structure_is_rejected(self) -> None:
        self.write("src/structure/interval/incremental_interval_scheduling.hpp")
        self.assertTrue(any("expected src/algorithm/other/scheduling" in item for item in self.messages()))

    def test_graph_algorithm_in_structure_is_rejected(self) -> None:
        self.write(
            "src/structure/graph/dynamic_connectivity/"
            "coordinate_product_knight_distances.hpp"
        )
        self.assertTrue(any(
            "expected src/algorithm/graph/shortest_path" in item
            for item in self.messages()
        ))

    def test_legacy_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/example.test.cpp",
            '#include "../../src/structure/other/sparse_table.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_legacy_dsu_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/dsu.test.cpp",
            '#include "../../src/structure/dsu/dsu.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_legacy_range_query_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/range.test.cpp",
            '#include "../../src/structure/range_query/sparse_table.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_relative_legacy_types_include_is_rejected(self) -> None:
        self.write(
            "src/structure/segtree/basic/example.hpp",
            '#include "../../types/monoid.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_legacy_graph_structure_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/graph.test.cpp",
            '#include "../../src/structure/graph/online_dynamic_connectivity.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_legacy_graph_algorithm_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/graph.test.cpp",
            '#include "../../src/structure/graph/'
            'coordinate_product_knight_distances.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_relative_legacy_graph_include_is_rejected(self) -> None:
        self.write(
            "src/structure/array/example.hpp",
            '#include "../graph/online_dynamic_connectivity.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_relative_legacy_dsu_include_is_rejected(self) -> None:
        self.write(
            "src/structure/graph/example.hpp",
            '#include "../dsu/rollback_dsu.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_legacy_tree_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/tree.test.cpp",
            '#include "../../src/structure/tree/link_cut_tree.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_legacy_bbst_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/bbst.test.cpp",
            '#include "../../src/structure/bbst/red_black_tree.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_legacy_convex_hull_trick_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/cht.test.cpp",
            '#include "../../src/structure/convex_hull_trick/dynamic_li_chao_tree.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))


    def test_legacy_heap_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/heap.test.cpp",
            '#include "../../src/structure/heap/persistent_leftist_heap.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_legacy_trie_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/trie.test.cpp",
            '#include "../../src/structure/trie/trie.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_relative_legacy_tree_include_is_rejected(self) -> None:
        self.write(
            "src/structure/dsu/example.hpp",
            '#include "../tree/link_cut_tree.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))

    def test_relative_legacy_include_is_rejected(self) -> None:
        self.write(
            "src/structure/array/example.hpp",
            '#include "../other/sparse_table.hpp"\n',
        )
        self.assertTrue(any("legacy reference" in item for item in self.messages()))


if __name__ == "__main__":
    unittest.main()
