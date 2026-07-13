#!/usr/bin/env python3

from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

import filter_competitive_verifier_result as result_filter
import run_competitive_verifier_shards as runner


def file_result(status: str = "success") -> dict[str, object]:
    return {
        "verifications": [{"status": status, "elapsed": 0.1}],
        "newest": True,
    }


def verification_plan(*paths: str, empty: tuple[str, ...] = ()) -> str:
    files = {path: {"verification": [{"type": "test"}]} for path in paths}
    files.update({path: {"verification": []} for path in empty})
    return json.dumps({"files": files}) + "\n"


SUCCESS_VERIFIER = r"""#!/usr/bin/env python3
import argparse
import json

parser = argparse.ArgumentParser()
parser.add_argument("command")
parser.add_argument("--verify-json")
parser.add_argument("--output")
parser.add_argument("--prev-result")
parser.add_argument("--split", type=int, required=True)
parser.add_argument("--split-index", type=int, required=True)
args = parser.parse_args()
with open(args.output, "w", encoding="utf-8") as stream:
    json.dump({
        "total_seconds": 1,
        "files": {
            "present.cpp": {
                "verifications": [{"status": "success", "elapsed": 0.1}],
                "newest": True,
            }
        },
    }, stream)
"""


def write_executable(path: Path, source: str) -> None:
    path.write_text(source, encoding="utf-8")
    path.chmod(path.stat().st_mode | 0o111)


class FilterCompetitiveVerifierResultTest(unittest.TestCase):
    def test_cli_removes_deleted_and_empty_plan_entries(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            plan = root / "verify-files.json"
            result_path = root / "result.json"
            plan.write_text(
                verification_plan(
                    "./test/nested/../keep.cpp",
                    empty=("test/removed.cpp",),
                ),
                encoding="utf-8",
            )
            result_path.write_text(
                json.dumps(
                    {
                        "total_seconds": 3,
                        "files": {
                            "test/keep.cpp": file_result(),
                            "test/removed.cpp": file_result(),
                            "test/abc277_e.test.cpp": file_result(),
                        },
                    }
                ),
                encoding="utf-8",
            )

            status = result_filter.main(
                [
                    "--verify-json",
                    str(plan),
                    "--result",
                    str(result_path),
                ]
            )

            self.assertEqual(status, 0)
            filtered = runner.read_result(result_path)
            self.assertEqual(len(filtered.files), 1)
            self.assertEqual(
                {path.as_posix() for path in filtered.files}, {"test/keep.cpp"}
            )
            self.assertEqual(filtered.total_seconds, 3)

    def test_outside_repository_plan_path_is_rejected(self) -> None:
        with self.assertRaisesRegex(result_filter.ResultFilterError, "outside"):
            result_filter.planned_verification_paths(
                {"files": {"../outside.cpp": {"verification": [{}]}}},
                root=Path.cwd(),
            )

    def test_missing_planned_result_fails_but_saves_partial_result(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            verifier = root / "fake-verifier"
            plan = root / "verify-files.json"
            output = root / "result.json"
            shards = root / "shards"
            write_executable(verifier, SUCCESS_VERIFIER)

            plan.write_text(verification_plan("present.cpp"), encoding="utf-8")
            common_arguments = [
                "--verifier",
                str(verifier),
                "--verify-json",
                str(plan),
                "--output",
                str(output),
                "--shard-dir",
                str(shards),
                "--jobs",
                "1",
            ]
            self.assertEqual(runner.main(common_arguments), 0)

            plan.write_text(
                verification_plan("present.cpp", "missing.cpp"), encoding="utf-8"
            )
            self.assertEqual(runner.main(common_arguments), 1)

            partial = runner.read_result(output)
            self.assertTrue(partial.is_success())
            self.assertEqual(
                {path.as_posix() for path in partial.files}, {"present.cpp"}
            )


if __name__ == "__main__":
    unittest.main()
