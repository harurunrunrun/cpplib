#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from check_test_verifier_markers import find_violations


class TestVerifierMarkerCheckTest(unittest.TestCase):
    def make_root(self) -> tuple[tempfile.TemporaryDirectory[str], Path]:
        temporary = tempfile.TemporaryDirectory()
        root = Path(temporary.name)
        (root / "test" / "onlinejudge").mkdir(parents=True)
        (root / "test" / "standalone").mkdir(parents=True)
        return temporary, root

    def write_valid_pair(self, root: Path) -> None:
        (root / "test" / "onlinejudge" / "sample.test.cpp").write_text(
            "// competitive-verifier: PROBLEM https://example.com/problem\n"
            "int main(){}\n",
            encoding="utf-8",
        )
        (root / "test" / "standalone" / "sample.test.cpp").write_text(
            "// competitive-verifier: STANDALONE\nint main(){}\n",
            encoding="utf-8",
        )

    def test_accepts_comment_problem_and_early_standalone_directives(self) -> None:
        temporary, root = self.make_root()
        with temporary:
            self.write_valid_pair(root)
            self.assertEqual(find_violations(root), [])

    def test_rejects_missing_duplicate_and_legacy_problem_directives(self) -> None:
        temporary, root = self.make_root()
        with temporary:
            self.write_valid_pair(root)
            onlinejudge = root / "test" / "onlinejudge"
            (onlinejudge / "missing.test.cpp").write_text(
                "int main(){}\n",
                encoding="utf-8",
            )
            (onlinejudge / "duplicate.test.cpp").write_text(
                "// competitive-verifier: PROBLEM https://example.com/a\n"
                "// competitive-verifier: PROBLEM https://example.com/b\n",
                encoding="utf-8",
            )
            (onlinejudge / "legacy.test.cpp").write_text(
                '#define PROBLEM "https://example.com/legacy"\n',
                encoding="utf-8",
            )

            violations = find_violations(root)
            self.assertTrue(any("missing.test.cpp" in item for item in violations))
            self.assertTrue(any("duplicate.test.cpp" in item for item in violations))
            self.assertTrue(
                any("legacy #define PROBLEM" in item for item in violations)
            )

    def test_rejects_standalone_directive_after_first_ten_lines(self) -> None:
        temporary, root = self.make_root()
        with temporary:
            self.write_valid_pair(root)
            late = root / "test" / "standalone" / "late.test.cpp"
            late.write_text(
                "\n" * 10 + "// competitive-verifier: STANDALONE\n",
                encoding="utf-8",
            )
            self.assertTrue(
                any("late.test.cpp" in item for item in find_violations(root))
            )


if __name__ == "__main__":
    unittest.main()
