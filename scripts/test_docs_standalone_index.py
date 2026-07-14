#!/usr/bin/env python3

from __future__ import annotations

import unittest
from pathlib import Path

import yaml


ROOT = Path(__file__).resolve().parent.parent
STANDALONE_MARKER = "// competitive-verifier: STANDALONE"


class DocsStandaloneIndexTest(unittest.TestCase):
    def test_docs_workflow_resolves_standalone_tests(self) -> None:
        workflow = yaml.load(
            (ROOT / ".github/workflows/docs.yaml").read_text(encoding="utf-8"),
            Loader=yaml.BaseLoader,
        )
        steps = workflow["jobs"]["docs"]["steps"]
        resolve_steps = [
            step
            for step in steps
            if step.get("uses", "").endswith("/oj-resolve@v2")
        ]
        self.assertEqual(len(resolve_steps), 1)
        include = resolve_steps[0]["with"]["include"].splitlines()
        self.assertEqual(include, ["src", "test/onlinejudge", "test/standalone"])

    def test_verify_workflow_does_not_resolve_standalone_tests_twice(self) -> None:
        workflow = yaml.load(
            (ROOT / ".github/workflows/verify.yaml").read_text(encoding="utf-8"),
            Loader=yaml.BaseLoader,
        )
        steps = workflow["jobs"]["resolve"]["steps"]
        resolve_steps = [
            step
            for step in steps
            if step.get("uses", "").endswith("/oj-resolve@v2")
        ]
        self.assertEqual(len(resolve_steps), 1)
        include = resolve_steps[0]["with"]["include"].splitlines()
        self.assertEqual(include, ["src", "test/onlinejudge"])

    def test_local_docs_resolve_is_separate_and_includes_standalone(self) -> None:
        makefile = (ROOT / "Makefile").read_text(encoding="utf-8")
        self.assertIn("docs-verifier-resolve:", makefile)
        commands = [
            line
            for line in makefile.splitlines()
            if "$(VERIFIER) oj-resolve" in line
        ]
        self.assertEqual(len(commands), 2)
        verify_commands = [line for line in commands if "$(VERIFY_FILES).tmp" in line]
        docs_commands = [line for line in commands if "$(DOCS_VERIFY_FILES).tmp" in line]
        self.assertEqual(len(verify_commands), 1)
        self.assertEqual(len(docs_commands), 1)
        self.assertIn("--include src test/onlinejudge", verify_commands[0])
        self.assertNotIn("test/standalone", verify_commands[0])
        self.assertIn(
            "--include src test/onlinejudge test/standalone", docs_commands[0]
        )

    def test_every_standalone_test_has_verifier_marker(self) -> None:
        tests = sorted((ROOT / "test/standalone").glob("*.test.cpp"))
        self.assertTrue(tests)
        missing = []
        for path in tests:
            first_lines = path.read_text(encoding="utf-8").splitlines()[:10]
            if STANDALONE_MARKER not in first_lines:
                missing.append(path.relative_to(ROOT).as_posix())
        self.assertEqual(missing, [])


if __name__ == "__main__":
    unittest.main()
