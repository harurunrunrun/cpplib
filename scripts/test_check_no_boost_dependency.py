#!/usr/bin/env python3
"""Tests for check_no_boost_dependency.py."""

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from check_no_boost_dependency import find_violations


class CheckNoBoostDependencyTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary_directory.cleanup)
        self.root = Path(self.temporary_directory.name)
        self.src = self.root / "src"
        self.test = self.root / "test"
        self.docs = self.root / "docs"
        for directory in (self.src, self.test, self.docs):
            directory.mkdir()

    def write(self, relative_path: str, body: str) -> Path:
        path = self.root / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(body, encoding="utf-8")
        return path

    def scan(self) -> tuple[list, int]:
        return find_violations([self.src, self.test, self.docs])

    def test_accepts_standard_library_and_non_identifier_substrings(self) -> None:
        self.write(
            "src/algorithm/example.hpp",
            "#include <vector>\nint boosted_value = 0;\n",
        )
        self.write(
            "test/standalone/example.test.cpp",
            "// turbo_boosted is an ordinary identifier.\nint main() {}\n",
        )
        self.write("docs/example.md", "A boosted search heuristic.\n")

        violations, count = self.scan()
        self.assertEqual(violations, [])
        self.assertEqual(count, 3)

    def test_rejects_angle_and_quoted_includes_with_flexible_spacing(self) -> None:
        self.write(
            "src/a.hpp",
            "#include <boost/multiprecision/cpp_int.hpp>\n",
        )
        self.write(
            "test/a.cpp",
            '# include  "boost/graph/adjacency_list.hpp"\n',
        )

        violations, _ = self.scan()
        self.assertEqual(
            [item.rule for item in violations],
            ["boost-reference", "boost-reference"],
        )

    def test_rejects_namespace_alias_using_directive_and_macro(self) -> None:
        self.write(
            "src/names.hpp",
            "namespace mp = boost :: multiprecision;\n"
            "using namespace boost;\n"
            "namespace boost {}\n"
            "#if BOOST_VERSION > 0\n#endif\n",
        )

        violations, _ = self.scan()
        self.assertEqual(len(violations), 4)
        self.assertTrue(all(item.rule == "boost-reference" for item in violations))
        self.assertEqual([item.line for item in violations], [1, 2, 3, 4])

    def test_rejects_references_in_tests_and_documentation(self) -> None:
        self.write(
            "test/standalone/example.test.cpp",
            "using Exact = boost::multiprecision::cpp_int;\n",
        )
        self.write(
            "docs/example.md",
            "The implementation uses `Boost::multiprecision`.\n",
        )

        violations, _ = self.scan()
        self.assertEqual(len(violations), 2)
        self.assertEqual(
            {item.logical_path.as_posix() for item in violations},
            {"test/standalone/example.test.cpp", "docs/example.md"},
        )

    def test_rejects_boost_path_components_but_not_longer_words(self) -> None:
        self.write("src/algorithm/boost_adapter.hpp", "#pragma once\n")
        self.write("src/algorithm/boosted_adapter.hpp", "#pragma once\n")

        violations, count = self.scan()
        self.assertEqual(count, 2)
        self.assertEqual(len(violations), 1)
        self.assertEqual(violations[0].rule, "boost-path")
        self.assertEqual(
            violations[0].logical_path.as_posix(),
            "src/algorithm/boost_adapter.hpp",
        )

    def test_scans_future_cpp_extensions_and_ignores_unrelated_files(self) -> None:
        suffixes = (
            ".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx",
            ".inc", ".ipp", ".tpp",
        )
        for index, suffix in enumerate(suffixes):
            self.write(f"src/nested/file_{index}{suffix}", "boost::type value;\n")
        self.write("test/generator/example.py", "value = 'boost::not_cpp'\n")
        self.write("docs/ignored.txt", "boost::not_documentation\n")

        violations, count = self.scan()
        self.assertEqual(count, len(suffixes))
        self.assertEqual(len(violations), len(suffixes))

    def test_reports_deterministic_paths_lines_and_columns(self) -> None:
        self.write("src/z.hpp", "first\nusing X = boost::type;\n")
        self.write("src/a.hpp", "boost::first; boost::second;\n")

        violations, _ = self.scan()
        self.assertEqual(
            [
                (item.logical_path.as_posix(), item.line, item.column)
                for item in violations
            ],
            [
                ("src/a.hpp", 1, 1),
                ("src/a.hpp", 1, 15),
                ("src/z.hpp", 2, 11),
            ],
        )

    def test_invalid_utf8_and_missing_roots_fail_closed(self) -> None:
        invalid = self.src / "invalid.hpp"
        invalid.write_bytes(b"\xff")
        missing = self.root / "missing"

        violations, count = find_violations([self.src, missing])
        self.assertEqual(count, 1)
        self.assertEqual(
            [(item.rule, item.logical_path) for item in violations],
            [("missing-root", missing), ("read-error", Path("src/invalid.hpp"))],
        )


if __name__ == "__main__":
    unittest.main()
