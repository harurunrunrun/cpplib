#!/usr/bin/env python3

from __future__ import annotations

import copy
import json
import tempfile
import unittest
from pathlib import Path

import normalize_competitive_verifier_plan as normalizer


def file_entry(problem: str, verifications: list[dict[str, object]]) -> dict[str, object]:
    return {
        "dependencies": ["src/example.hpp"],
        "verification": verifications,
        "document_attributes": {"PROBLEM": problem},
    }


class NormalizePlanTest(unittest.TestCase):
    def test_removes_only_atcoder_constant_failures(self) -> None:
        atcoder_const = {"type": "const", "status": "failure"}
        atcoder_problem = {
            "type": "problem",
            "problem": "https://atcoder.jp/contests/abc999/tasks/abc999_a",
        }
        const_success = {"type": "const", "status": "success"}
        plan = {
            "files": {
                "test/onlinejudge/atcoder.test.cpp": file_entry(
                    "https://atcoder.jp/contests/abc999/tasks/abc999_a",
                    [atcoder_const, atcoder_problem, const_success],
                ),
                "test/onlinejudge/yosupo.test.cpp": file_entry(
                    "https://judge.yosupo.jp/problem/aplusb",
                    [{"type": "problem", "problem": "https://judge.yosupo.jp/problem/aplusb"}],
                ),
            },
            "metadata": {"keep": True},
        }

        normalized, removed = normalizer.normalize_plan(plan)

        self.assertEqual(removed, ["test/onlinejudge/atcoder.test.cpp"])
        self.assertEqual(
            normalized["files"]["test/onlinejudge/atcoder.test.cpp"]["verification"],
            [atcoder_problem, const_success],
        )
        self.assertEqual(normalized["metadata"], {"keep": True})
        self.assertEqual(len(plan["files"]["test/onlinejudge/atcoder.test.cpp"]["verification"]), 3)

    def test_accepts_atcoder_subdomain_and_case(self) -> None:
        plan = {
            "files": {
                "test.cpp": file_entry(
                    "https://WWW.AtCoder.JP./contests/example/tasks/example_a",
                    [{"type": "const", "status": "failure"}],
                )
            }
        }
        normalized, removed = normalizer.normalize_plan(plan)
        self.assertEqual(removed, ["test.cpp"])
        self.assertEqual(normalized["files"]["test.cpp"]["verification"], [])

    def test_rejects_non_atcoder_constant_failure_without_mutating_input(self) -> None:
        plan = {
            "files": {
                "test.cpp": file_entry(
                    "https://atcoder.jp.example.com/problem/a",
                    [{"type": "const", "status": "failure"}],
                )
            }
        }
        original = copy.deepcopy(plan)
        with self.assertRaisesRegex(normalizer.PlanNormalizationError, "unexpected constant"):
            normalizer.normalize_plan(plan)
        self.assertEqual(plan, original)

    def test_known_unsupported_hosts_remove_only_minimal_failure(self) -> None:
        urls = (
            "https://atcoder.jp/contests/example/tasks/example_a",
            "https://spoj.com/problems/EXAMPLE/",
            "https://www.spoj.com/problems/EXAMPLE/",
            "https://br.spoj.com/problems/EXAMPLE/",
            "https://codechef.com/problems/EXAMPLE",
            "https://www.codechef.com/problems/EXAMPLE",
            "https://open.kattis.com/problems/example",
        )
        for url in urls:
            with self.subTest(url=url):
                const_success = {"type": "const", "status": "success"}
                problem_verification = {"type": "problem", "problem": url}
                plan = {
                    "files": {
                        "test.cpp": file_entry(
                            url,
                            [
                                {"type": "const", "status": "failure"},
                                const_success,
                                problem_verification,
                            ],
                        )
                    }
                }
                normalized, removed = normalizer.normalize_plan(plan)
                self.assertEqual(removed, ["test.cpp"])
                self.assertEqual(
                    normalized["files"]["test.cpp"]["verification"],
                    [const_success, problem_verification],
                )

    def test_rejects_unknown_and_lookalike_hosts(self) -> None:
        urls = (
            "https://spoj.com.example.com/problems/A/",
            "https://codechef.com.example.com/problems/A",
            "https://example-spoj.com/problems/A/",
            "https://judge.yosupo.jp/problem/aplusb",
            "https://open.kattis.com.example.com/problems/a",
        )
        for url in urls:
            with self.subTest(url=url):
                plan = {
                    "files": {
                        "test.cpp": file_entry(
                            url,
                            [{"type": "const", "status": "failure"}],
                        )
                    }
                }
                with self.assertRaisesRegex(
                    normalizer.PlanNormalizationError, "unexpected constant"
                ):
                    normalizer.normalize_plan(plan)

    def test_rejects_richer_known_host_constant_failures(self) -> None:
        for url in (
            "https://www.spoj.com/problems/EXAMPLE/",
            "https://br.spoj.com/problems/EXAMPLE/",
            "https://www.codechef.com/problems/EXAMPLE",
        ):
            with self.subTest(url=url):
                plan = {
                    "files": {
                        "test.cpp": file_entry(
                            url,
                            [
                                {
                                    "type": "const",
                                    "status": "failure",
                                    "reason": "compilation failed",
                                }
                            ],
                        )
                    }
                }
                with self.assertRaisesRegex(
                    normalizer.PlanNormalizationError, "unexpected constant"
                ):
                    normalizer.normalize_plan(plan)

    def test_rejects_missing_problem_url(self) -> None:
        plan = {
            "files": {
                "test.cpp": {
                    "verification": [{"type": "const", "status": "failure"}]
                }
            }
        }
        with self.assertRaisesRegex(normalizer.PlanNormalizationError, "no PROBLEM URL"):
            normalizer.normalize_plan(plan)

    def test_rejects_richer_atcoder_constant_failure(self) -> None:
        plan = {
            "files": {
                "test.cpp": file_entry(
                    "https://atcoder.jp/contests/example/tasks/example_a",
                    [
                        {
                            "type": "const",
                            "status": "failure",
                            "reason": "compilation failed",
                        }
                    ],
                )
            }
        }
        with self.assertRaisesRegex(normalizer.PlanNormalizationError, "unexpected constant"):
            normalizer.normalize_plan(plan)

    def test_conflicting_problem_urls_are_not_hidden(self) -> None:
        plan = {
            "files": {
                "test.cpp": {
                    "verification": [
                        {
                            "type": "const",
                            "status": "failure",
                            "problem": "https://judge.yosupo.jp/problem/aplusb",
                        }
                    ],
                    "document_attributes": {
                        "PROBLEM": "https://atcoder.jp/contests/example/tasks/example_a"
                    },
                }
            }
        }
        with self.assertRaises(normalizer.PlanNormalizationError):
            normalizer.normalize_plan(plan)

    def test_cli_replaces_plan_atomically(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "verify-files.json"
            path.write_text(
                json.dumps(
                    {
                        "files": {
                            "test.cpp": file_entry(
                                "https://atcoder.jp/contests/example/tasks/example_a",
                                [{"type": "const", "status": "failure"}],
                            )
                        }
                    }
                ),
                encoding="utf-8",
            )
            self.assertEqual(normalizer.main([str(path)]), 0)
            normalized = json.loads(path.read_text(encoding="utf-8"))
            self.assertEqual(normalized["files"]["test.cpp"]["verification"], [])

    def test_rejects_malformed_plan(self) -> None:
        with self.assertRaisesRegex(normalizer.PlanNormalizationError, "files object"):
            normalizer.normalize_plan({"files": []})


if __name__ == "__main__":
    unittest.main()
