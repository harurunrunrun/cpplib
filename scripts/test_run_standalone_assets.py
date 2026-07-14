#!/usr/bin/env python3

from __future__ import annotations

import io
import os
import tempfile
import unittest
from unittest import mock
from contextlib import contextmanager, redirect_stderr
from pathlib import Path

import run_standalone_assets as runner
from standalone_verification_results import load_manifest, result_path


@contextmanager
def working_directory(path: Path):
    previous = Path.cwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(previous)


GENERATOR = """\\
import argparse
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", type=Path, required=True)
args = parser.parse_args()
args.out_dir.mkdir(parents=True, exist_ok=True)
(args.out_dir / "case.in").write_text("generated\\n", encoding="utf-8")
"""

CHECKER = """\\
import argparse
import json
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--test", type=Path, required=True)
parser.add_argument("--case-dir", type=Path, required=True)
parser.add_argument("--build-dir", type=Path, required=True)
parser.add_argument("--cxx", required=True)
parser.add_argument("--cxxflags", required=True)
args = parser.parse_args()
name = args.test.name[:-len(".test.cpp")]
manifest = json.loads((Path("results") / f"{name}.json").read_text(encoding="utf-8"))
if manifest["status"] != "running":
    raise SystemExit(1)
if (args.case_dir / "case.in").read_text(encoding="utf-8") != "generated\\n":
    raise SystemExit(1)
args.build_dir.mkdir(parents=True, exist_ok=True)
(args.build_dir / "checked").write_text(args.test.name + "\\n", encoding="utf-8")
"""

EMPTY_GENERATOR = """\\
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("--out-dir")
parser.parse_args()
"""
EMPTY_CHECKER = "import argparse\nargparse.ArgumentParser().parse_known_args()\n"


def add_test(root: Path, name: str) -> None:
    (root / "test" / "standalone").mkdir(parents=True, exist_ok=True)
    (root / "test" / "generator" / name).mkdir(parents=True, exist_ok=True)
    (root / "test" / "checker" / name).mkdir(parents=True, exist_ok=True)
    (root / "test" / "standalone" / f"{name}.test.cpp").write_text(
        "// fake test\\n", encoding="utf-8"
    )
    (root / "test" / "generator" / name / "generator.py").write_text(
        GENERATOR, encoding="utf-8"
    )
    (root / "test" / "checker" / name / "checker.py").write_text(
        CHECKER, encoding="utf-8"
    )


class RunStandaloneAssetsTest(unittest.TestCase):
    def test_split_processes_exact_disjoint_slice(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            names = ["alpha", "bravo", "charlie", "delta", "echo"]
            for name in names:
                add_test(root, name)

            with working_directory(root):
                status = runner.main(
                    [
                        "--cache-dir",
                        "cache",
                        "--result-dir",
                        "results",
                        "--split-size",
                        "2",
                        "--split-index",
                        "1",
                    ]
                )

            self.assertEqual(status, 0)
            checked = sorted(
                path.parent.parent.name
                for path in root.glob("cache/*/build/checked")
            )
            self.assertEqual(checked, names[1::2])
            for name in names[1::2]:
                manifest = load_manifest(
                    result_path(root / "results", f"test/standalone/{name}.test.cpp")
                )
                self.assertEqual(manifest["status"], "success")
                self.assertEqual(manifest["case_count"], 1)
                self.assertGreaterEqual(manifest["elapsed_seconds"], 0)

    def test_missing_asset_keeps_failure_exit_code(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            add_test(root, "missing_checker")
            (root / "test" / "checker" / "missing_checker" / "checker.py").unlink()

            output = io.StringIO()
            with working_directory(root), redirect_stderr(output):
                status = runner.main(
                    [
                        "--cache-dir",
                        "cache",
                        "--result-dir",
                        "results",
                    ]
                )

            self.assertEqual(status, 1)
            manifest = load_manifest(
                result_path(
                    root / "results",
                    "test/standalone/missing_checker.test.cpp",
                )
            )
            self.assertEqual(manifest["status"], "failure")
            self.assertEqual(manifest["case_count"], 1)

    def test_zero_generated_cases_fail_even_when_checker_succeeds(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            name = "empty"
            add_test(root, name)
            (root / "test" / "generator" / name / "generator.py").write_text(
                EMPTY_GENERATOR, encoding="utf-8"
            )
            (root / "test" / "checker" / name / "checker.py").write_text(
                EMPTY_CHECKER, encoding="utf-8"
            )

            output = io.StringIO()
            with working_directory(root), redirect_stderr(output):
                status = runner.main(
                    [
                        "--cache-dir",
                        "cache",
                        "--result-dir",
                        "results",
                    ]
                )

            self.assertEqual(status, 1)
            manifest = load_manifest(
                result_path(
                    root / "results", "test/standalone/empty.test.cpp"
                )
            )
            self.assertEqual(manifest["status"], "failure")
            self.assertEqual(manifest["case_count"], 0)
            self.assertIn("generator produced no .in cases", output.getvalue())

    def test_fingerprint_failure_is_annotated(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            output = io.StringIO()
            with (
                working_directory(root),
                redirect_stderr(output),
                mock.patch.object(
                    runner,
                    "source_fingerprint",
                    side_effect=OSError("unreadable source"),
                ),
            ):
                status = runner.main([])
            self.assertEqual(status, 1)
            self.assertIn("standalone-assets%3A fingerprint", output.getvalue())

    def test_invalid_split_is_rejected(self) -> None:
        with self.assertRaises(SystemExit) as context:
            runner.main(["--split-size", "0"])
        self.assertEqual(context.exception.code, 2)

    def test_failure_annotation_is_safely_escaped(self) -> None:
        output = io.StringIO()
        with redirect_stderr(output):
            runner.report_failure_annotation(
                "bad:name,100%",
                "first line\r\nsecond: line, 100%",
            )

        self.assertEqual(
            output.getvalue(),
            "::error title=standalone-assets%3A bad%3Aname%2C100%25::"
            "first line%0D%0Asecond%3A line%2C 100%25\n",
        )


if __name__ == "__main__":
    unittest.main()
