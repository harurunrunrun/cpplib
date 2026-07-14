#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from competitive_verifier_docs_result import build_docs_result
from standalone_verification_results import (
    make_manifest,
    result_path,
    source_fingerprint,
    write_manifest,
)


def plan_entry(*, standalone: bool) -> dict:
    attributes = {"STANDALONE": ""} if standalone else {}
    return {
        "verification": [{"name": "g++"}],
        "document_attributes": attributes,
    }


class CompetitiveVerifierDocsResultTest(unittest.TestCase):
    def test_merges_only_current_successes_inside_the_docs_plan(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            standalone = root / "test" / "standalone"
            standalone.mkdir(parents=True)
            names = ("accepted", "stale", "failed", "outside")
            paths = {}
            for name in names:
                path = standalone / f"{name}.test.cpp"
                path.write_text("int main() { return 0; }\n", encoding="utf-8")
                paths[name] = path.relative_to(root).as_posix()
            result_dir = root / "standalone-results"
            fingerprint = source_fingerprint(root)

            manifests = {
                "accepted": make_manifest(
                    test_path=paths["accepted"],
                    fingerprint=fingerprint,
                    status="success",
                    elapsed_seconds=1.5,
                    case_count=2,
                    executed_at="2026-01-02T03:04:05Z",
                ),
                "stale": make_manifest(
                    test_path=paths["stale"],
                    fingerprint="0" * 64,
                    status="success",
                    case_count=1,
                ),
                "failed": make_manifest(
                    test_path=paths["failed"],
                    fingerprint=fingerprint,
                    status="failure",
                    case_count=1,
                ),
                "outside": make_manifest(
                    test_path=paths["outside"],
                    fingerprint=fingerprint,
                    status="success",
                    case_count=1,
                ),
            }
            for name, manifest in manifests.items():
                write_manifest(result_path(result_dir, paths[name]), manifest)

            online_path = "test/onlinejudge/example.test.cpp"
            verify_files = {
                "files": {
                    online_path: plan_entry(standalone=False),
                    paths["accepted"]: plan_entry(standalone=True),
                    paths["stale"]: plan_entry(standalone=True),
                    paths["failed"]: plan_entry(standalone=True),
                    "src/example.hpp": {
                        "verification": [],
                        "document_attributes": {},
                    },
                }
            }
            online_file_result = {
                "verifications": [
                    {
                        "verification_name": "g++",
                        "status": "success",
                        "elapsed": 5.0,
                        "last_execution_time": "2026-01-01T00:00:00Z",
                    }
                ],
                "newest": False,
            }
            forged_standalone_result = {
                "verifications": [
                    {
                        "verification_name": "g++",
                        "status": "success",
                        "elapsed": 9.0,
                        "last_execution_time": "2026-01-01T00:00:00Z",
                    }
                ],
                "newest": False,
            }
            online_result = {
                "total_seconds": 5.0,
                "files": {
                    online_path: online_file_result,
                    paths["stale"]: forged_standalone_result,
                    "test/onlinejudge/outside.test.cpp": online_file_result,
                },
            }

            result = build_docs_result(
                verify_files,
                online_result,
                root=root,
                standalone_results=result_dir,
            )

            self.assertEqual(set(result["files"]), set(verify_files["files"]) - {"src/example.hpp"})
            self.assertEqual(result["files"][online_path], online_file_result)
            accepted = result["files"][paths["accepted"]]
            self.assertTrue(accepted["newest"])
            self.assertEqual(accepted["verifications"][0]["status"], "success")
            self.assertEqual(
                accepted["verifications"][0]["testcases"][0]["name"],
                "2 generated case(s)",
            )
            self.assertEqual(result["files"][paths["stale"]]["verifications"], [])
            self.assertEqual(result["files"][paths["failed"]]["verifications"], [])
            self.assertNotIn(paths["outside"], result["files"])
            self.assertEqual(result["total_seconds"], 6.5)

    def test_without_standalone_results_leaves_standalone_waiting(self) -> None:
        path = "test/standalone/example.test.cpp"
        result = build_docs_result(
            {"files": {path: plan_entry(standalone=True)}},
            {"total_seconds": 0, "files": {}},
            root=Path("."),
            standalone_results=None,
        )
        self.assertEqual(result["files"][path]["verifications"], [])


if __name__ == "__main__":
    unittest.main()
