#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

import check_standalone_verification_results as checker
from standalone_verification_results import (
    make_manifest,
    result_path,
    source_fingerprint,
    write_manifest,
)


class CheckStandaloneVerificationResultsTest(unittest.TestCase):
    def test_requires_every_current_success(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            standalone = root / "test" / "standalone"
            standalone.mkdir(parents=True)
            tests = []
            for name in ("alpha", "bravo"):
                path = standalone / f"{name}.test.cpp"
                path.write_text("int main() { return 0; }\n", encoding="utf-8")
                tests.append(path.relative_to(root).as_posix())
            result_dir = root / "results"
            fingerprint = source_fingerprint(root)
            write_manifest(
                result_path(result_dir, tests[0]),
                make_manifest(
                    test_path=tests[0],
                    fingerprint=fingerprint,
                    status="success",
                    case_count=1,
                ),
            )
            failures = checker.check_results(root, result_dir)
            self.assertTrue(any(tests[1] in failure for failure in failures))

            write_manifest(
                result_path(result_dir, tests[1]),
                make_manifest(
                    test_path=tests[1],
                    fingerprint=fingerprint,
                    status="success",
                    case_count=1,
                ),
            )
            self.assertEqual(checker.check_results(root, result_dir), [])


if __name__ == "__main__":
    unittest.main()
