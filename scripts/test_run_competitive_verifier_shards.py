#!/usr/bin/env python3

from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

import run_competitive_verifier_shards as runner
from test_filter_competitive_verifier_result import FilterCompetitiveVerifierResultTest


def file_result(status: str, *, newest: bool) -> dict[str, object]:
    return {
        "verifications": [{"status": status, "elapsed": 0.1}],
        "newest": newest,
    }


FAKE_VERIFIER = r"""#!/usr/bin/env python3
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

if args.command != "verify":
    raise SystemExit(64)
if args.split_index == 0:
    files = {
        "passed-zero.cpp": {
            "verifications": [{"status": "success", "elapsed": 0.1}],
            "newest": True,
        }
    }
    status = 0
else:
    files = {
        "passed-one.cpp": {
            "verifications": [{"status": "success", "elapsed": 0.2}],
            "newest": True,
        },
        "failed-one.cpp": {
            "verifications": [{"status": "failure", "elapsed": 0.3}],
            "newest": True,
        },
    }
    status = 1
with open(args.output, "w", encoding="utf-8") as stream:
    json.dump({"total_seconds": 1, "files": files}, stream)
raise SystemExit(status)
"""

CRASHING_VERIFIER = r"""#!/usr/bin/env python3
import argparse
import json

parser = argparse.ArgumentParser()
parser.add_argument("command")
parser.add_argument("--verify-json")
parser.add_argument("--output")
parser.add_argument("--split", type=int, required=True)
parser.add_argument("--split-index", type=int, required=True)
args = parser.parse_args()
if args.split_index:
    raise SystemExit(3)
with open(args.output, "w", encoding="utf-8") as stream:
    json.dump({
        "total_seconds": 1,
        "files": {
            "fresh.cpp": {
                "verifications": [{"status": "success", "elapsed": 0.1}],
                "newest": True,
            }
        },
    }, stream)
"""


PREVIOUS_INSPECTING_VERIFIER = r"""#!/usr/bin/env python3
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
with open(args.prev_result, encoding="utf-8") as stream:
    previous = json.load(stream)
if set(previous["files"]) != {"passed.cpp"}:
    raise SystemExit(9)
with open(args.output, "w", encoding="utf-8") as stream:
    json.dump({
        "total_seconds": 1,
        "files": {
            path: {
                "verifications": [{"status": "success", "elapsed": 0.1}],
                "newest": True,
            }
            for path in ("passed.cpp", "failed.cpp")
        },
    }, stream)
"""


def write_executable(path: Path, source: str) -> None:
    path.write_text(source, encoding="utf-8")
    path.chmod(path.stat().st_mode | 0o111)


def verification_plan(*paths: str, empty: tuple[str, ...] = ()) -> str:
    files = {path: {"verification": [{"type": "test"}]} for path in paths}
    files.update({path: {"verification": []} for path in empty})
    return json.dumps({"files": files}) + "\n"


class RunCompetitiveVerifierShardsTest(unittest.TestCase):
    def test_previous_result_is_planned_and_success_only_for_verifier(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            verifier = root / "fake-verifier"
            plan = root / "verify-files.json"
            output = root / "result.json"
            previous = root / "previous.json"
            shards = root / "shards"
            write_executable(verifier, PREVIOUS_INSPECTING_VERIFIER)
            plan.write_text(
                verification_plan("passed.cpp", "failed.cpp"), encoding="utf-8"
            )
            previous.write_text(
                json.dumps(
                    {
                        "total_seconds": 0,
                        "files": {
                            "passed.cpp": file_result("success", newest=False),
                            "failed.cpp": file_result("failure", newest=True),
                            "stale.cpp": file_result("success", newest=True),
                        },
                    }
                ),
                encoding="utf-8",
            )

            status = runner.main(
                [
                    "--verifier",
                    str(verifier),
                    "--verify-json",
                    str(plan),
                    "--output",
                    str(output),
                    "--prev-result",
                    str(previous),
                    "--shard-dir",
                    str(shards),
                    "--jobs",
                    "1",
                ]
            )

            self.assertEqual(status, 0)
            normalized = runner.read_result(shards / "previous-success.json")
            self.assertEqual(
                {path.as_posix() for path in normalized.files}, {"passed.cpp"}
            )
            original = runner.read_result(previous)
            self.assertEqual(len(original.files), 3)

    def test_merge_keeps_successes_when_another_file_fails(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            first = root / "first.json"
            second = root / "second.json"
            output = root / "result.json"
            first.write_text(
                json.dumps(
                    {
                        "total_seconds": 1.0,
                        "files": {
                            "success.cpp": file_result("success", newest=True),
                            "updated.cpp": file_result("failure", newest=False),
                        },
                    }
                ),
                encoding="utf-8",
            )
            second.write_text(
                json.dumps(
                    {
                        "total_seconds": 2.0,
                        "files": {
                            "updated.cpp": file_result("success", newest=True),
                            "failure.cpp": file_result("failure", newest=True),
                        },
                    }
                ),
                encoding="utf-8",
            )

            merged = runner.merge_result_files([first, second])
            runner.write_result_atomic(output, merged)
            reparsed = runner.read_result(output)

            self.assertEqual(reparsed.total_seconds, 3.0)
            self.assertEqual(
                {path.as_posix() for path in reparsed.files},
                {"success.cpp", "updated.cpp", "failure.cpp"},
            )
            self.assertEqual(
                reparsed.files[Path("updated.cpp")].verifications[0].status.value,
                "success",
            )
            self.assertFalse(reparsed.is_success())

    def test_later_newest_result_wins(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            paths: list[Path] = []
            for index, status in enumerate(("failure", "success")):
                path = root / f"result-{index}.json"
                path.write_text(
                    json.dumps(
                        {
                            "total_seconds": 0,
                            "files": {
                                "same.cpp": file_result(status, newest=True)
                            },
                        }
                    ),
                    encoding="utf-8",
                )
                paths.append(path)
            merged = runner.merge_result_files(paths)
            self.assertTrue(merged.is_success())

    def test_empty_merge_is_rejected(self) -> None:
        with self.assertRaisesRegex(ValueError, "no shard results"):
            runner.merge_result_files([])

    def test_fake_verifier_merges_passes_and_preserves_failure_exit(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            verifier = root / "fake-verifier"
            plan = root / "verify-files.json"
            output = root / "result.json"
            previous = root / "previous.json"
            write_executable(verifier, FAKE_VERIFIER)
            plan.write_text(
                verification_plan("passed-zero.cpp", "passed-one.cpp", "failed-one.cpp"),
                encoding="utf-8",
            )
            previous.write_text(
                json.dumps(
                    {
                        "total_seconds": 0,
                        "files": {"old.cpp": file_result("success", newest=False)},
                    }
                ),
                encoding="utf-8",
            )

            status = runner.main(
                [
                    "--verifier",
                    str(verifier),
                    "--verify-json",
                    str(plan),
                    "--output",
                    str(output),
                    "--prev-result",
                    str(previous),
                    "--shard-dir",
                    str(root / "shards"),
                    "--jobs",
                    "2",
                ]
            )

            self.assertEqual(status, 1)
            result = runner.read_result(output)
            self.assertEqual(
                {path.as_posix() for path in result.files},
                {"passed-zero.cpp", "passed-one.cpp", "failed-one.cpp"},
            )
            self.assertEqual(
                result.files[Path("passed-one.cpp")].verifications[0].status.value,
                "success",
            )
            self.assertFalse(result.is_success())

    def test_crashed_shard_cannot_reuse_stale_output(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            verifier = root / "fake-verifier"
            plan = root / "verify-files.json"
            output = root / "result.json"
            shards = root / "shards"
            shards.mkdir()
            write_executable(verifier, CRASHING_VERIFIER)
            plan.write_text(verification_plan("fresh.cpp"), encoding="utf-8")
            (shards / "result-1.json").write_text(
                json.dumps(
                    {
                        "total_seconds": 1,
                        "files": {"stale.cpp": file_result("success", newest=True)},
                    }
                ),
                encoding="utf-8",
            )

            status = runner.main(
                [
                    "--verifier",
                    str(verifier),
                    "--verify-json",
                    str(plan),
                    "--output",
                    str(output),
                    "--shard-dir",
                    str(shards),
                    "--jobs",
                    "2",
                ]
            )

            self.assertEqual(status, 1)
            result = runner.read_result(output)
            self.assertEqual(
                {path.as_posix() for path in result.files}, {"fresh.cpp"}
            )
            self.assertFalse((shards / "result-1.json").exists())


if __name__ == "__main__":
    unittest.main()
