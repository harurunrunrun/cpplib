#!/usr/bin/env python3
"""Tests for check_problem_independent_library_names.py."""

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from check_problem_independent_library_names import (
    CASE_INSENSITIVE_PROBLEM_CODES,
    OLD_PUBLIC_IDENTIFIERS,
    PROBLEM_CODES,
    check_file,
    public_files,
)


class CheckProblemIndependentLibraryNamesTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary_directory.cleanup)
        self.root = Path(self.temporary_directory.name)

    def write(self, relative_path: str, body: str) -> Path:
        path = self.root / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(body, encoding="utf-8")
        return path

    def valid_doc(
        self,
        title: str = "Bipartite Graph Test (二部グラフ判定) [BUGLIFE]",
    ) -> str:
        return (
            "---\n"
            f"title: {title}\n"
            "documentation_of: ../src/algorithm/graph/constraints/is_bipartite.hpp\n"
            "---\n\n"
            "無向グラフが二部グラフかを判定する。\n"
        )

    def terms(self, path: Path, *, doc: bool | None = None) -> list[str]:
        return [
            violation.term
            for violation in check_file(path, is_documentation=doc)
        ]

    def test_generic_title_may_have_trailing_verification_tag(self) -> None:
        path = self.write("docs/is_bipartite.md", self.valid_doc())
        self.assertEqual(self.terms(path), [])

    def test_multiple_tags_and_quoted_title_are_allowed(self) -> None:
        path = self.write(
            "docs/minimum_spanning_tree.md",
            self.valid_doc(
                '"Minimum Spanning Tree (最小全域木) [CSTREET] [GRL_2_A]"'
            ),
        )
        self.assertEqual(self.terms(path), [])

    def test_every_known_problem_code_is_allowed_only_as_a_title_tag(self) -> None:
        for code in PROBLEM_CODES:
            with self.subTest(code=code):
                path = self.write(
                    "docs/generic_operation.md",
                    self.valid_doc(
                        f"Generic Graph Operation (一般グラフ操作) [{code}]"
                    ),
                )
                self.assertEqual(self.terms(path), [])
                path.write_text(
                    self.valid_doc().replace(
                        "無向グラフが二部グラフかを判定する。",
                        f"{code} の問題を解く。",
                    ),
                    encoding="utf-8",
                )
                self.assertIn(code, self.terms(path))

    def test_distinctive_problem_codes_are_rejected_in_lowercase_apis(self) -> None:
        for code in CASE_INSENSITIVE_PROBLEM_CODES:
            with self.subTest(code=code):
                identifier = code.lower() + "_answer"
                path = self.write(
                    f"src/algorithm/other/{identifier}.hpp",
                    f"void {identifier}();\n",
                )
                violations = check_file(path, is_documentation=False)
                self.assertTrue(
                    any(
                        item.rule == "problem-code" and item.context == "path"
                        for item in violations
                    )
                )
                self.assertTrue(
                    any(
                        item.rule == "problem-code"
                        and item.context == "public source"
                        for item in violations
                    )
                )

    def test_ordinary_lowercase_acquire_api_is_not_a_problem_code(self) -> None:
        path = self.write(
            "src/structure/other/acquire_lock.hpp",
            "void acquire_lock();\n",
        )
        self.assertEqual(self.terms(path, doc=False), [])

    def test_lowercase_fisher_story_is_rejected(self) -> None:
        path = self.write(
            "docs/story.md",
            self.valid_doc().replace("無向グラフ", "fisher"),
        )
        self.assertTrue(
            any(
                item.rule == "problem-story"
                and item.context == "documentation"
                and item.term == "fisher"
                for item in check_file(path)
            )
        )
        header = self.write(
            "src/algorithm/other/fisher_answer.hpp",
            "void fisher_answer();\n",
        )
        violations = check_file(header, is_documentation=False)
        self.assertTrue(
            any(
                item.rule == "problem-code" and item.context == "path"
                for item in violations
            )
        )
        self.assertTrue(
            any(
                item.rule == "problem-code" and item.context == "public source"
                for item in violations
            )
        )

    def test_fisher_yates_standard_algorithm_is_allowed(self) -> None:
        header = self.write(
            "src/algorithm/other/fisher_yates_shuffle.hpp",
            "// Fisher-Yates and Fisher–Yates shuffle.\n"
            "void fisher_yates_shuffle();\n",
        )
        self.assertEqual(self.terms(header, doc=False), [])
        document = self.write(
            "docs/fisher_yates_shuffle.md",
            self.valid_doc(
                "Fisher-Yates Shuffle (フィッシャー–イェーツシャッフル)"
            ).replace("無向グラフ", "Fisher–Yates shuffle"),
        )
        self.assertEqual(self.terms(document), [])

    def test_generated_include_guard_is_not_public_vocabulary(self) -> None:
        header = self.write(
            "src/approximate/packing/bottom_left_packing.hpp",
            "#ifndef CPPLIB_SRC_APPROXIMATE_PACKING_BOTTOM_LEFT_PACKING_HPP_INCLUDED\n"
            "#define CPPLIB_SRC_APPROXIMATE_PACKING_BOTTOM_LEFT_PACKING_HPP_INCLUDED\n"
            "inline int bottom_left_packing(){ return 0; }\n"
            "#endif  // CPPLIB_SRC_APPROXIMATE_PACKING_BOTTOM_LEFT_PACKING_HPP_INCLUDED\n",
        )
        self.assertEqual(self.terms(header, doc=False), [])
        header.write_text(
            header.read_text(encoding="utf-8").replace(
                "inline int bottom_left_packing(){ return 0; }",
                "inline const char* code(){ return \"BOTTOM\"; }",
            ),
            encoding="utf-8",
        )
        self.assertIn("BOTTOM", self.terms(header, doc=False))

    def test_problem_story_in_title_is_rejected_before_allowed_tag(self) -> None:
        path = self.write(
            "docs/is_bipartite.md",
            self.valid_doc(
                "Bug Interactions Are Consistent (虫の相互作用整合性) [BUGLIFE]"
            ),
        )
        terms = self.terms(path)
        self.assertIn("Bug Interactions", terms)
        self.assertIn("虫の相互作用", terms)
        self.assertNotIn("BUGLIFE", terms)

    def test_problem_tag_is_rejected_outside_trailing_title_position(self) -> None:
        path = self.write(
            "docs/is_bipartite.md",
            self.valid_doc().replace(
                "無向グラフが二部グラフかを判定する。",
                "BUGLIFE の入力を処理する。",
            ),
        )
        self.assertIn("BUGLIFE", self.terms(path))

        path = self.write(
            "docs/misplaced_tag.md",
            self.valid_doc("Bipartite [BUGLIFE] Graph Test (二部グラフ判定)"),
        )
        self.assertIn("BUGLIFE", self.terms(path))

    def test_online_judge_brand_and_url_are_rejected(self) -> None:
        header = self.write(
            "src/algorithm/graph/shortest_path.hpp",
            "// Accepted by SPOJ.\n"
            "// https://judge.yosupo.jp/problem/shortest_path\n",
        )
        terms = self.terms(header, doc=False)
        self.assertIn("SPOJ", terms)
        self.assertTrue(any("judge.yosupo.jp" in term for term in terms))

        adapter = self.write(
            "src/algorithm/graph/spoj_adapter.hpp",
            "void spoj_adapter();\n",
        )
        violations = check_file(adapter, is_documentation=False)
        self.assertTrue(
            any(
                item.rule == "online-judge-reference" and item.context == "path"
                for item in violations
            )
        )
        self.assertTrue(
            any(
                item.rule == "online-judge-reference"
                and item.context == "public source"
                for item in violations
            )
        )

    def test_retired_identifier_is_rejected_in_path_and_content(self) -> None:
        path = self.write(
            "src/algorithm/graph/maximum_taxi_pickups.hpp",
            "int maximum_taxi_pickups();\n",
        )
        violations = check_file(path, is_documentation=False)
        self.assertTrue(any(item.context == "path" for item in violations))
        self.assertTrue(any(item.context == "public source" for item in violations))
        self.assertIn("maximum_taxi_pickups", [item.term for item in violations])

    def test_retired_identifier_in_documentation_of_is_rejected(self) -> None:
        path = self.write(
            "docs/generic_matching.md",
            self.valid_doc().replace(
                "../src/algorithm/graph/constraints/is_bipartite.hpp",
                "../src/algorithm/matching/maximum_taxi_pickups.hpp",
            ),
        )
        violations = check_file(path)
        self.assertTrue(
            any(
                item.rule == "retired-problem-api"
                and item.context == "documentation"
                for item in violations
            )
        )

    def test_every_retired_public_identifier_is_rejected(self) -> None:
        for identifier in OLD_PUBLIC_IDENTIFIERS:
            with self.subTest(identifier=identifier):
                path = self.write(
                    "src/algorithm/other/generic.hpp",
                    f"void {identifier}();\n",
                )
                violations = check_file(path, is_documentation=False)
                self.assertTrue(
                    any(item.rule == "retired-problem-api" for item in violations)
                )

    def test_japanese_story_vocabulary_is_rejected(self) -> None:
        path = self.write(
            "docs/story.md",
            self.valid_doc().replace(
                "無向グラフが二部グラフかを判定する。",
                "タクシーとペンギンを割り当てる。",
            ),
        )
        terms = self.terms(path)
        self.assertIn("タクシー", terms)
        self.assertIn("ペンギン", terms)

    def test_recent_retired_story_phrases_are_rejected(self) -> None:
        phrases = (
            "Bottom Vertices",
            "有向グラフの底頂点",
            "Critical Edges",
            "切断辺列挙",
            "Fishmonger",
        )
        for phrase in phrases:
            with self.subTest(phrase=phrase):
                path = self.write(
                    "docs/story.md",
                    self.valid_doc().replace(
                        "無向グラフが二部グラフかを判定する。",
                        phrase,
                    ),
                )
                self.assertTrue(
                    any(item.rule == "problem-story" for item in check_file(path))
                )

    def test_established_algorithm_name_is_not_a_problem_story(self) -> None:
        path = self.write(
            "docs/chinese_postman.md",
            self.valid_doc(
                "Chinese Postman Problem (中国人郵便配達問題)"
            ).replace(
                "無向グラフが二部グラフかを判定する。",
                "全辺を少なくとも一度通る最短閉路を求める。",
            ),
        )
        self.assertEqual(self.terms(path), [])

    def test_public_file_collection_matches_the_documented_scope(self) -> None:
        header = self.write("src/algorithm/a.hpp", "#pragma once\n")
        nested_header = self.write("src/structure/x/b.hpp", "#pragma once\n")
        self.write("src/algorithm/not_public.cpp", "")
        document = self.write("docs/a.md", self.valid_doc())
        self.write("docs/nested/ignored.md", self.valid_doc())

        headers, documents = public_files(self.root / "src", self.root / "docs")
        self.assertEqual(headers, [header, nested_header])
        self.assertEqual(documents, [document])


if __name__ == "__main__":
    unittest.main()
