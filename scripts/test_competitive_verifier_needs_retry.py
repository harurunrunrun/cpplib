#!/usr/bin/env python3

from __future__ import annotations

import json
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

import competitive_verifier_needs_retry as retry

SCRIPT = Path(__file__).with_name("competitive_verifier_needs_retry.py")


def result(*statuses: str) -> dict[str, object]:
    return {
        "total_seconds": 1.0,
        "files": {
            f"test/onlinejudge/{index}.test.cpp": {
                "verifications": [{"status": status, "elapsed": 0.1}],
                "newest": True,
            }
            for index, status in enumerate(statuses)
        },
    }


class NeedsRetryTest(unittest.TestCase):
    def write(self, directory: str, data: object) -> Path:
        path = Path(directory) / "result.json"
        path.write_text(json.dumps(data), encoding="utf-8")
        return path

    def test_all_success_does_not_retry(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = self.write(directory, result("success", "success"))
            self.assertFalse(retry.needs_retry(path, "true")[0])

    def test_skipped_by_timeout_retries_even_if_action_succeeded(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = self.write(directory, result("success", "skipped"))
            needed, reason = retry.needs_retry(path, "true")
            self.assertTrue(needed)
            self.assertIn("1 unfinished", reason)

    def test_failure_retries(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = self.write(directory, result("failure"))
            self.assertTrue(retry.needs_retry(path, "true")[0])

    def test_action_failure_retries(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = self.write(directory, result("success"))
            self.assertTrue(retry.needs_retry(path, "false")[0])
            self.assertTrue(retry.needs_retry(path, "")[0])

    def test_missing_or_invalid_result_retries(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            missing = Path(directory) / "missing.json"
            self.assertTrue(retry.needs_retry(missing, "true")[0])
            invalid = Path(directory) / "invalid.json"
            invalid.write_text("{", encoding="utf-8")
            self.assertTrue(retry.needs_retry(invalid, "true")[0])
            no_files = self.write(directory, {"total_seconds": 0})
            self.assertTrue(retry.needs_retry(no_files, "true")[0])

    def test_writes_github_output(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = self.write(directory, result("success", "skipped"))
            output = Path(directory) / "github_output"
            subprocess.run(
                [sys.executable, str(SCRIPT), "--result", str(path), "--is-success", "true"],
                check=True,
                capture_output=True,
                env={**os.environ, "GITHUB_OUTPUT": str(output)},
            )
            self.assertEqual(output.read_text(encoding="utf-8"), "needs_retry=true\n")


if __name__ == "__main__":
    unittest.main()
