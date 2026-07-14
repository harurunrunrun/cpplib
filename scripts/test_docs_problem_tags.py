#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from docs_problem_tags import problem_tag, update_docs


class DocsProblemTagsTest(unittest.TestCase):
    def test_problem_tag_for_supported_judges(self) -> None:
        self.assertEqual(
            problem_tag("https://www.spoj.com/problems/QTREE/"), "QTREE"
        )
        self.assertEqual(
            problem_tag("https://br.spoj.com/SPOJ/problems/QTREE6/"), "QTREE6"
        )
        self.assertEqual(
            problem_tag("https://atcoder.jp/contests/abc294/tasks/abc294_g"),
            "ABC294_G",
        )
        self.assertEqual(
            problem_tag("https://onlinejudge.u-aizu.ac.jp/problems/GRL_5_C"),
            "GRL_5_C",
        )
        self.assertEqual(
            problem_tag("https://judge.yosupo.jp/problem/static_range_sum"),
            "static_range_sum",
        )

    def test_unrelated_and_lookalike_hosts_are_ignored(self) -> None:
        self.assertIsNone(problem_tag("https://example.com/problems/QTREE/"))
        self.assertIsNone(problem_tag("https://spoj.com.example.org/problems/X/"))

    def test_write_and_check_are_idempotent(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "src" / "tree.hpp"
            docs = root / "docs"
            onlinejudge = root / "test" / "onlinejudge"
            source.parent.mkdir(parents=True)
            docs.mkdir()
            onlinejudge.mkdir(parents=True)
            source.write_text("#pragma once\n", encoding="utf-8")
            document = docs / "tree.md"
            document.write_text(
                "---\n"
                "title: Tree (木) [OLD]\n"
                "documentation_of: ../src/tree.hpp\n"
                "---\n",
                encoding="utf-8",
            )
            (onlinejudge / "qtree.test.cpp").write_text(
                "// competitive-verifier: PROBLEM https://www.spoj.com/problems/QTREE/\n"
                '#include "../../src/tree.hpp"\n',
                encoding="utf-8",
            )
            (onlinejudge / "tree_path.test.cpp").write_text(
                '#define PROBLEM "https://judge.yosupo.jp/problem/tree_path_composite_sum"\n'
                '#include "../../src/tree.hpp"\n',
                encoding="utf-8",
            )

            self.assertEqual(update_docs(docs, onlinejudge, write=True), [])
            self.assertIn(
                "title: Tree (木) [QTREE] [tree_path_composite_sum]",
                document.read_text(encoding="utf-8"),
            )
            self.assertEqual(update_docs(docs, onlinejudge, write=False), [])


    def test_quoted_front_matter_is_preserved_and_body_fields_are_ignored(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "src" / "tree.hpp"
            docs = root / "docs"
            onlinejudge = root / "test" / "onlinejudge"
            source.parent.mkdir(parents=True)
            docs.mkdir()
            onlinejudge.mkdir(parents=True)
            source.write_text("#pragma once\n", encoding="utf-8")
            japanese_tree = "\u6728"
            document = docs / "tree.md"
            document.write_text(
                "---\n"
                f'title: "Tree ({japanese_tree}) [OLD]"\n'
                "documentation_of: '../src/tree.hpp'\n"
                "---\n"
                "title: Body title must stay unchanged\n"
                "documentation_of: ../src/not-front-matter.hpp\n",
                encoding="utf-8",
            )
            (onlinejudge / "qtree.test.cpp").write_text(
                "// competitive-verifier: PROBLEM "
                "https://www.spoj.com/problems/QTREE/\n"
                '#include "../../src/tree.hpp"\n',
                encoding="utf-8",
            )

            self.assertEqual(update_docs(docs, onlinejudge, write=True), [])
            updated = document.read_text(encoding="utf-8")
            self.assertIn(
                f'title: "Tree ({japanese_tree}) [QTREE]"',
                updated,
            )
            self.assertIn("title: Body title must stay unchanged", updated)
            self.assertIn(
                "documentation_of: ../src/not-front-matter.hpp", updated
            )
            self.assertEqual(update_docs(docs, onlinejudge, write=False), [])


if __name__ == "__main__":
    unittest.main()
