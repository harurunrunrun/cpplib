#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from standalone_verification_results import (
    current_successes,
    load_manifest,
    make_manifest,
    result_path,
    source_fingerprint,
    write_manifest,
)


class StandaloneVerificationResultsTest(unittest.TestCase):
    def fixture(self, directory: str) -> tuple[Path, Path, str]:
        root = Path(directory)
        test = root / "test" / "standalone" / "example.test.cpp"
        test.parent.mkdir(parents=True)
        test.write_text("int main() { return 0; }\n", encoding="utf-8")
        header = root / "src" / "example.hpp"
        header.parent.mkdir(parents=True)
        header.write_text("#pragma once\n", encoding="utf-8")
        result_dir = root / "results"
        return root, result_dir, test.relative_to(root).as_posix()

    def test_round_trip_current_success(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root, result_dir, test = self.fixture(directory)
            path = result_path(result_dir, test)
            write_manifest(
                path,
                make_manifest(
                    test_path=test,
                    fingerprint=source_fingerprint(root),
                    status="success",
                    elapsed_seconds=1.25,
                    case_count=3,
                    executed_at="2026-01-02T03:04:05Z",
                ),
            )
            self.assertEqual(load_manifest(path)["case_count"], 3)
            successes, issues = current_successes(root, result_dir)
            self.assertEqual(set(successes), {test})
            self.assertEqual(issues, [])

    def test_source_change_makes_result_stale(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root, result_dir, test = self.fixture(directory)
            write_manifest(
                result_path(result_dir, test),
                make_manifest(
                    test_path=test,
                    fingerprint=source_fingerprint(root),
                    status="success",
                ),
            )
            (root / "src" / "example.hpp").write_text(
                "#pragma once\ninline int value = 1;\n", encoding="utf-8"
            )
            successes, issues = current_successes(root, result_dir)
            self.assertEqual(successes, {})
            self.assertEqual(len(issues), 1)
            self.assertIn("stale result", issues[0])

    def test_makefile_change_makes_result_stale(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root, result_dir, test = self.fixture(directory)
            makefile = root / "Makefile"
            makefile.write_text("CXXFLAGS := -O2\n", encoding="utf-8")
            write_manifest(
                result_path(result_dir, test),
                make_manifest(
                    test_path=test,
                    fingerprint=source_fingerprint(root),
                    status="success",
                    case_count=1,
                ),
            )
            makefile.write_text(
                "CXXFLAGS := -O2 -Wall -Wextra\n",
                encoding="utf-8",
            )
            successes, issues = current_successes(root, result_dir)
            self.assertEqual(successes, {})
            self.assertEqual(len(issues), 1)
            self.assertIn("stale result", issues[0])

    def test_failure_is_not_reported_as_success(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root, result_dir, test = self.fixture(directory)
            write_manifest(
                result_path(result_dir, test),
                make_manifest(
                    test_path=test,
                    fingerprint=source_fingerprint(root),
                    status="failure",
                ),
            )
            successes, issues = current_successes(root, result_dir)
            self.assertEqual(successes, {})
            self.assertIn("non-success result", issues[0])

    def test_zero_case_success_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root, result_dir, test = self.fixture(directory)
            write_manifest(
                result_path(result_dir, test),
                make_manifest(
                    test_path=test,
                    fingerprint=source_fingerprint(root),
                    status="success",
                    case_count=0,
                ),
            )
            successes, issues = current_successes(root, result_dir)
            self.assertEqual(successes, {})
            self.assertEqual(len(issues), 1)
            self.assertIn("zero-case success", issues[0])


if __name__ == "__main__":
    unittest.main()
