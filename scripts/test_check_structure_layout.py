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
        self.write("src/structure/range_query/sparse_table.hpp")
        self.write("docs/structure/range_query/sparse_table.md")
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
        self.write("src/structure/ordered_set/integer_set.hpp")
        self.write("docs/structure/ordered_set/integer_set.md")
        self.write(
            "src/structure/convex_hull_trick/li_chao/dynamic_li_chao_tree.hpp"
        )
        self.write("docs/structure/convex_hull_trick/li_chao/dynamic_li_chao_tree.md")
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

    def test_flat_category_cannot_be_split_further(self) -> None:
        self.write("src/structure/range_query/static/sparse_table.hpp")
        self.assertTrue(any("must not have subcategories" in item for item in self.messages()))

    def test_known_header_in_wrong_category_is_rejected(self) -> None:
        self.write("src/structure/heap/sparse_table.hpp")
        self.assertTrue(any("expected src/structure/range_query" in item for item in self.messages()))

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
    def test_legacy_convex_hull_trick_include_is_rejected(self) -> None:
        self.write(
            "test/standalone/cht.test.cpp",
            '#include "../../src/structure/convex_hull_trick/dynamic_li_chao_tree.hpp"\n',
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
