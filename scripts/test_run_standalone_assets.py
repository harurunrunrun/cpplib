#!/usr/bin/env python3

from __future__ import annotations

import io
import os
import tempfile
import unittest
from contextlib import contextmanager, redirect_stderr
from pathlib import Path

import run_standalone_assets as runner


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
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--test", type=Path, required=True)
parser.add_argument("--case-dir", type=Path, required=True)
parser.add_argument("--build-dir", type=Path, required=True)
parser.add_argument("--cxx", required=True)
parser.add_argument("--cxxflags", required=True)
args = parser.parse_args()
if (args.case_dir / "case.in").read_text(encoding="utf-8") != "generated\\n":
    raise SystemExit(1)
args.build_dir.mkdir(parents=True, exist_ok=True)
(args.build_dir / "checked").write_text(args.test.name + "\\n", encoding="utf-8")
"""


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

    def test_missing_asset_keeps_failure_exit_code(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            add_test(root, "missing_checker")
            (root / "test" / "checker" / "missing_checker" / "checker.py").unlink()

            output = io.StringIO()
            with working_directory(root), redirect_stderr(output):
                status = runner.main(["--cache-dir", "cache"])

            self.assertEqual(status, 1)

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
