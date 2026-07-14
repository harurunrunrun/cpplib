#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from check_unsupported_onlinejudge_assets import find_violations


def add_wrapper(root: Path, filename: str, url: str) -> Path:
    wrapper = root / "test" / "onlinejudge" / filename
    wrapper.parent.mkdir(parents=True, exist_ok=True)
    wrapper.write_text(f"// competitive-verifier: PROBLEM {url}\n", encoding="utf-8")
    return wrapper


def add_assets(root: Path, name: str, wrapper: Path) -> None:
    standalone = root / "test" / "standalone" / f"{name}.test.cpp"
    generator = root / "test" / "generator" / name / "generator.py"
    checker = root / "test" / "checker" / name / "checker.py"
    for path in (standalone, generator, checker):
        path.parent.mkdir(parents=True, exist_ok=True)
    standalone.write_text(
        "// competitive-verifier: STANDALONE\n\n"
        f'#include "../onlinejudge/{wrapper.name}"\n',
        encoding="utf-8",
    )
    generator.write_text("# generator\n", encoding="utf-8")
    checker.write_text("# checker\n", encoding="utf-8")


class UnsupportedOnlinejudgeAssetsTest(unittest.TestCase):
    def test_supported_judge_is_ignored(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            add_wrapper(
                root,
                "unionfind.test.cpp",
                "https://judge.yosupo.jp/problem/unionfind",
            )
            self.assertEqual(find_violations(root), [])

    def test_arbitrary_and_lookalike_urls_are_ignored(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = root / "test" / "onlinejudge" / "example.test.cpp"
            wrapper.parent.mkdir(parents=True, exist_ok=True)
            wrapper.write_text(
                "// https://www.spoj.com/problems/IGNORED/\n"
                "// competitive-verifier: PROBLEM "
                "https://spoj.com.example.org/problem/X\n",
                encoding="utf-8",
            )
            self.assertEqual(find_violations(root), [])

    def test_missing_assets_are_reported_for_subdomains(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            add_wrapper(
                root,
                "qtree6.dynamic_tree.test.cpp",
                "https://br.spoj.com/SPOJ/problems/QTREE6/",
            )
            violations = find_violations(root)
            self.assertEqual(len(violations), 3)
            self.assertTrue(
                all("qtree6.dynamic_tree.test.cpp" in item for item in violations)
            )

    def test_complete_assets_are_accepted(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = add_wrapper(
                root,
                "abc001_a.example.test.cpp",
                "https://atcoder.jp/contests/abc001/tasks/abc001_a",
            )
            add_assets(root, "offline_atcoder_abc001_a", wrapper)
            self.assertEqual(find_violations(root), [])

    def test_kattis_assets_are_accepted(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = add_wrapper(
                root,
                "cats.answer.test.cpp",
                "https://open.kattis.com/problems/cats",
            )
            add_assets(root, "offline_kattis_cats", wrapper)
            self.assertEqual(find_violations(root), [])

    def test_wrong_include_and_missing_marker_are_reported(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = add_wrapper(
                root,
                "flipcoin.adapter.test.cpp",
                "https://www.codechef.com/problems/FLIPCOIN",
            )
            add_assets(root, "offline_codechef_flipcoin", wrapper)
            standalone = (
                root
                / "test"
                / "standalone"
                / "offline_codechef_flipcoin.test.cpp"
            )
            standalone.write_text(
                f"// {wrapper.name}\n#include \"../onlinejudge/wrong.test.cpp\"\n",
                encoding="utf-8",
            )
            violations = find_violations(root)
            self.assertEqual(len(violations), 2)
            self.assertTrue(any("STANDALONE" in item for item in violations))
            self.assertTrue(any("does not include" in item for item in violations))

    def test_duplicate_expected_name_is_reported(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            add_wrapper(root, "same.one.test.cpp", "https://www.spoj.com/problems/ONE/")
            add_wrapper(root, "same.two.test.cpp", "https://www.spoj.com/problems/TWO/")
            violations = find_violations(root)
            self.assertTrue(any("also belongs" in item for item in violations))


if __name__ == "__main__":
    unittest.main()
