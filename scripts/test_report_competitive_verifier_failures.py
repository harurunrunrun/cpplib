#!/usr/bin/env python3

from __future__ import annotations

import io
import json
import tempfile
import unittest
from contextlib import redirect_stderr
from pathlib import Path

from competitive_verifier.models import VerifyCommandResult

import report_competitive_verifier_failures as reporter


class ReportCompetitiveVerifierFailuresTest(unittest.TestCase):
    def test_failure_details_include_path_status_and_failed_case(self) -> None:
        result = VerifyCommandResult.model_validate(
            {
                "total_seconds": 1,
                "files": {
                    "pass.cpp": {
                        "verifications": [
                            {"status": "success", "elapsed": 0.1}
                        ]
                    },
                    "fail.cpp": {
                        "verifications": [
                            {
                                "verification_name": "g++",
                                "status": "failure",
                                "elapsed": 0.2,
                                "testcases": [
                                    {
                                        "name": "sample",
                                        "status": "WA",
                                        "elapsed": 0.1,
                                    }
                                ],
                            }
                        ]
                    },
                    "skip.cpp": {
                        "verifications": [
                            {"status": "skipped", "elapsed": 0.0}
                        ]
                    },
                },
            }
        )
        details = reporter.failure_details(result)
        self.assertEqual([detail.path.as_posix() for detail in details], [
            "fail.cpp",
            "skip.cpp",
        ])
        self.assertIn("sample (WA)", details[0].message)
        self.assertEqual(details[1].status, "skipped")

    def test_cli_emits_file_annotations_and_plain_diagnostics(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            result_path = Path(directory) / "result.json"
            result_path.write_text(
                json.dumps(
                    {
                        "total_seconds": 0,
                        "files": {
                            "test/fail.cpp": {
                                "verifications": [
                                    {"status": "failure", "elapsed": 0.1}
                                ]
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )
            output = io.StringIO()
            with redirect_stderr(output):
                status = reporter.main(["--result", str(result_path)])
            self.assertEqual(status, 0)
            rendered = output.getvalue()
            self.assertIn("[competitive-verifier] test/fail.cpp", rendered)
            self.assertIn("::error file=test/fail.cpp", rendered)


if __name__ == "__main__":
    unittest.main()
