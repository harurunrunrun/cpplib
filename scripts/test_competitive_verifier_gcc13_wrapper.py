#!/usr/bin/env python3
"""Unit tests for the competitive-verifier GCC 13 compiler wrapper."""

from __future__ import annotations

import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest


SCRIPTS_DIR = Path(__file__).resolve().parent
WRAPPER = SCRIPTS_DIR / "competitive_verifier_gcc13" / "g++"


class CompetitiveVerifierGcc13WrapperTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary_directory.cleanup)
        self.root = Path(self.temporary_directory.name)
        self.log = self.root / "arguments.json"
        self.compiler = self.root / "real-g++"
        self.compiler.write_text(
            f"#!{sys.executable}\n"
            "import json\n"
            "import os\n"
            "from pathlib import Path\n"
            "import sys\n"
            "Path(os.environ['FAKE_GXX_LOG']).write_text(\n"
            "    json.dumps(sys.argv[1:]), encoding='utf-8'\n"
            ")\n"
            "raise SystemExit(int(os.environ.get('FAKE_GXX_EXIT', '0')))\n",
            encoding="utf-8",
        )
        self.compiler.chmod(0o755)

    def environment(self, *, exit_code: int = 0) -> dict[str, str]:
        environment = os.environ.copy()
        environment["COMPETITIVE_VERIFIER_REAL_GXX"] = str(self.compiler)
        environment["FAKE_GXX_LOG"] = str(self.log)
        environment["FAKE_GXX_EXIT"] = str(exit_code)
        return environment

    def invoke(
        self, arguments: list[str], *, environment: dict[str, str]
    ) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [str(WRAPPER), *arguments],
            env=environment,
            check=False,
            capture_output=True,
            text=True,
        )

    def recorded_arguments(self) -> list[str]:
        return json.loads(self.log.read_text(encoding="utf-8"))

    def test_injects_only_for_library_checker_source_and_propagates_exit(self) -> None:
        reference = (
            self.root
            / "cache"
            / "library-checker-problems"
            / "discrete_logarithm_mod"
            / "sol"
            / "reference.cpp"
        )
        result = self.invoke(
            ["-O2", "-c", str(reference)], environment=self.environment(exit_code=43)
        )

        self.assertEqual(result.returncode, 43)
        self.assertEqual(
            self.recorded_arguments(),
            ["-O2", "-c", str(reference), "-Wno-ignored-attributes"],
        )

    def test_does_not_inject_for_user_source_or_library_checker_include(self) -> None:
        user_source = self.root / "test" / "onlinejudge" / "example.test.cpp"
        library_checker_include = (
            self.root / "cache" / "library-checker-problems" / "common"
        )
        arguments = [str(user_source), f"-I{library_checker_include}", "-Werror"]
        result = self.invoke(arguments, environment=self.environment())

        self.assertEqual(result.returncode, 0)
        self.assertEqual(self.recorded_arguments(), arguments)

    def test_does_not_duplicate_existing_suppression(self) -> None:
        reference = (
            self.root / "library-checker-problems" / "problem" / "sol" / "main.cc"
        )
        arguments = [str(reference), "-Wno-ignored-attributes"]
        result = self.invoke(arguments, environment=self.environment())

        self.assertEqual(result.returncode, 0)
        self.assertEqual(self.recorded_arguments(), arguments)

    def test_missing_real_compiler_fails_without_recursing(self) -> None:
        environment = os.environ.copy()
        environment.pop("COMPETITIVE_VERIFIER_REAL_GXX", None)
        result = self.invoke([], environment=environment)

        self.assertEqual(result.returncode, 127)
        self.assertIn("COMPETITIVE_VERIFIER_REAL_GXX is not set", result.stderr)

    def test_self_recursive_real_compiler_is_rejected(self) -> None:
        environment = os.environ.copy()
        environment["COMPETITIVE_VERIFIER_REAL_GXX"] = str(WRAPPER)
        result = self.invoke([], environment=environment)

        self.assertEqual(result.returncode, 126)
        self.assertIn("resolves to this wrapper", result.stderr)


if __name__ == "__main__":
    unittest.main()
