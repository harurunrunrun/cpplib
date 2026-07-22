#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from docs_problem_tags import primary_sources


class DocsProblemTagsPrimarySourcesTest(unittest.TestCase):
    def test_descriptor_selects_primary_header(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = root / "test" / "onlinejudge" / "abc294_g.heavy_light_decomposition.test.cpp"
            wrapper.parent.mkdir(parents=True)
            source = (
                '#include "../../src/structure/fenwick_tree.hpp"\n'
                '#include "../../src/algorithm/tree/decomposition/heavy_light_decomposition.hpp"\n'
            )
            self.assertEqual(
                [path.stem for path in primary_sources(wrapper, source)],
                ["heavy_light_decomposition"],
            )

    def test_renamed_answer_api_alias_selects_direct_header(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = (
                root
                / "test"
                / "onlinejudge"
                / "treeiso.tree_isomorphism_answer.test.cpp"
            )
            direct = (
                root
                / "src"
                / "algorithm"
                / "tree"
                / "are_unrooted_trees_isomorphic.hpp"
            )
            backend = direct.parent / "tree_isomorphism.hpp"
            wrapper.parent.mkdir(parents=True)
            direct.parent.mkdir(parents=True)
            direct.write_text(
                '#include "tree_isomorphism.hpp"\n',
                encoding="utf-8",
            )
            backend.write_text("#pragma once\n", encoding="utf-8")
            source = (
                '#include "../../src/algorithm/tree/'
                'are_unrooted_trees_isomorphic.hpp"\n'
            )

            self.assertEqual(
                [path.stem for path in primary_sources(wrapper, source)],
                ["are_unrooted_trees_isomorphic"],
            )

    def test_helper_is_not_tagged_when_primary_is_unambiguous(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = root / "test" / "onlinejudge" / "range_affine_range_sum.test.cpp"
            wrapper.parent.mkdir(parents=True)
            source = (
                '#include "../../src/structure/modint/modint.hpp"\n'
                '#include "../../src/structure/segtree/lazysegtree.hpp"\n'
            )
            self.assertEqual(
                [path.stem for path in primary_sources(wrapper, source)],
                ["lazysegtree"],
            )


    def test_direct_and_transitive_helpers_are_not_tagged(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = (
                root
                / "test"
                / "onlinejudge"
                / "kgss.max_monoid_segtree.test.cpp"
            )
            segtree = root / "src" / "structure" / "segtree" / "segtree.hpp"
            common_monoids = (
                root / "src" / "structure" / "types" / "common_monoids.hpp"
            )
            monoid = root / "src" / "structure" / "types" / "monoid.hpp"
            wrapper.parent.mkdir(parents=True)
            segtree.parent.mkdir(parents=True)
            common_monoids.parent.mkdir(parents=True)
            segtree.write_text("#pragma once\n", encoding="utf-8")
            common_monoids.write_text(
                '#include "monoid.hpp"\n', encoding="utf-8"
            )
            monoid.write_text("#pragma once\n", encoding="utf-8")
            source = (
                '#include "../../src/structure/segtree/segtree.hpp"\n'
                '#include "../../src/structure/types/common_monoids.hpp"\n'
            )

            self.assertEqual(
                [path.stem for path in primary_sources(wrapper, source)],
                ["segtree"],
            )


if __name__ == "__main__":
    unittest.main()
