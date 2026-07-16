#!/usr/bin/env python3

from __future__ import annotations

import io
import tempfile
import unittest
from contextlib import redirect_stderr
from pathlib import Path

import prune_standalone_verification_results as pruner
from standalone_verification_results import make_manifest, write_manifest


FINGERPRINT = "0" * 64


def write_result(path: Path, test_path: str, *, status: str = "success") -> None:
    write_manifest(
        path,
        make_manifest(
            test_path=test_path,
            fingerprint=FINGERPRINT,
            status=status,
            case_count=1,
        ),
    )


class PruneStandaloneVerificationResultsTest(unittest.TestCase):
    def test_removes_only_valid_obsolete_runner_manifests(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            standalone = root / "test" / "standalone"
            standalone.mkdir(parents=True)
            (standalone / "current.test.cpp").write_text(
                "int main(){ return 0; }\n", encoding="utf-8"
            )

            results = root / "results"
            current = results / "current.json"
            obsolete = results / "renamed_old.json"
            mismatched = results / "not_the_manifest_name.json"
            malformed = results / "malformed_old.json"
            unrelated = results / "notes.txt"
            nested = results / "nested" / "obsolete.json"

            write_result(
                current,
                "test/standalone/current.test.cpp",
                status="running",
            )
            write_result(
                obsolete,
                "test/standalone/renamed_old.test.cpp",
                status="failure",
            )
            write_result(
                mismatched,
                "test/standalone/different_old_name.test.cpp",
            )
            malformed.write_text('{"test": "test/standalone/old.test.cpp"}\n', encoding="utf-8")
            unrelated.write_text("keep me\n", encoding="utf-8")
            nested.parent.mkdir(parents=True)
            write_result(nested, "test/standalone/obsolete.test.cpp")

            removed = pruner.prune_obsolete_results(root, results)

            self.assertEqual(removed, [obsolete.resolve()])
            self.assertTrue(current.exists())
            self.assertFalse(obsolete.exists())
            self.assertTrue(mismatched.exists())
            self.assertTrue(malformed.exists())
            self.assertTrue(unrelated.exists())
            self.assertTrue(nested.exists())

    def test_unlinks_obsolete_symlink_without_deleting_its_target(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "test" / "standalone").mkdir(parents=True)
            results = root / "results"
            results.mkdir()
            target = root / "external-manifest.json"
            write_result(target, "test/standalone/old.test.cpp")
            link = results / "old.json"
            link.symlink_to(target)

            removed = pruner.prune_obsolete_results(root, results)

            self.assertEqual(removed, [link.resolve(strict=False)])
            self.assertFalse(link.exists())
            self.assertFalse(link.is_symlink())
            self.assertTrue(target.exists())

    def test_missing_result_directory_is_a_successful_noop(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "test" / "standalone").mkdir(parents=True)
            output = io.StringIO()
            with redirect_stderr(output):
                status = pruner.main(
                    [
                        "--root",
                        str(root),
                        "--result-dir",
                        str(root / "missing"),
                    ]
                )

            self.assertEqual(status, 0)
            self.assertIn("removed 0 obsolete manifest(s)", output.getvalue())


if __name__ == "__main__":
    unittest.main()
