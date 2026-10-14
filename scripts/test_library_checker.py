#!/usr/bin/env python3

from __future__ import annotations

import json
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

import library_checker as lc


def problem(problem_id: str = "sample") -> dict:
    return {
        "id": problem_id, "directory": f"uncategorized/{problem_id}",
        "title": "Sample", "timelimit": 0.125,
        "tests": [{"name": "example.in", "number": 1}, {"name": "random.cpp", "number": 2}],
        "checker": "checker.cpp", "pinned_files": {},
    }


def add_driver(root: Path, name: str = "odd_filename.cpp", problem_id: str = "sample") -> str:
    path = root / "test/onlinejudge" / name
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(f"// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/{problem_id}\nint main(){{}}\n")
    return path.relative_to(root).as_posix()


def add_suite(repository: Path, entry: dict) -> dict[str, str]:
    hashes = {}
    directory = repository / entry["directory"]
    for case in lc.expected_cases(entry):
        for subdir, suffix in [("in", ".in"), ("out", ".out")]:
            path = directory / subdir / (case + suffix)
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(case + "\n")
            hashes[path.name] = lc.sha256(path)
    (directory / "hash.json").write_text(json.dumps(hashes))
    return hashes


class LibraryCheckerTest(unittest.TestCase):
    def test_discovery_includes_untracked_and_ignores_incidental_urls(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            first = add_driver(root)
            second = add_driver(root, "second.test.cpp")
            ignored = root / "test/onlinejudge/ignore.cpp"
            ignored.write_text("// https://judge.yosupo.jp/problem/other\n")
            self.assertEqual(lc.discover_drivers(root), {"sample": [first, second]})

    def test_discovery_supports_legacy_macro_and_rejects_multiple_problems(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            name = add_driver(root)
            path = root / name
            path.write_text('#define PROBLEM "https://judge.yosupo.jp/problem/legacy"\n')
            self.assertEqual(lc.discover_drivers(root), {"legacy": [name]})
            path.write_text(path.read_text() + "// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/other\n")
            with self.assertRaisesRegex(lc.LibraryCheckerError, "multiple"):
                lc.discover_drivers(root)

    def test_coverage_distinguishes_latest_inventory_and_cached_suites(self) -> None:
        catalog = {"revision": "old", "problems": [problem()]}
        inventory = {"revision": "new", "problems": [problem(), problem("uncategorized_new")]}
        result = lc.coverage_report(catalog, inventory, {"sample": ["one.cpp", "two.cpp"]})
        self.assertEqual((result["total"], result["covered"], result["driver_count"]), (2, 1, 2))
        self.assertEqual(result["missing"], ["uncategorized_new"])
        self.assertEqual(result["unavailable_suites"], ["uncategorized_new"])
        self.assertEqual(result["inventory_revision"], "new")
        self.assertEqual(result["suite_revision"], "old")
        with self.assertRaisesRegex(lc.LibraryCheckerError, "absent from pinned inventory"):
            lc.coverage_report(catalog, inventory, {"misspelled": ["driver.cpp"]})

    def test_supplement_metadata_preserves_its_own_revision(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            repository = root / "download"
            directory = repository / "new_category/new_problem"
            directory.mkdir(parents=True)
            (repository / "common").mkdir()
            (repository / "common/testlib.h").write_text("// trusted common header\n")
            (directory / "info.toml").write_text("title='New'\ntimelimit=3.0\n[[tests]]\nname='example.in'\nnumber=1\n")
            (directory / "hash.json").write_text("{}")
            (directory / "checker.cpp").write_text('#include "testlib.h"\n')
            revision = "1" * 40
            supplement = lc.build_supplement_catalog(repository, revision, ["new_problem"])
            self.assertEqual(supplement["problems"][0]["suite_revision"], revision)
            self.assertEqual(list(supplement["problems"][0]["support_files"]), ["common/testlib.h"])
            original_support = lc.sha256(repository / "common/testlib.h")
            lc.write_json(root / "test/library_checker/catalog.json", {
                "revision": "base", "problems": [problem()], "support_files": {"common/testlib.h": original_support}
            })
            lc.write_json(root / "test/library_checker/supplement_catalog.json", supplement)
            merged = lc.load_catalog(root)
            self.assertEqual(merged["revision"], "base")
            entry = next(entry for entry in merged["problems"] if entry["id"] == "new_problem")
            self.assertEqual(lc.suite_revision(merged, entry), revision)
            supplement["problems"][0]["id"] = "sample"
            lc.write_json(root / "test/library_checker/supplement_catalog.json", supplement)
            with self.assertRaisesRegex(lc.LibraryCheckerError, "duplicates base"):
                lc.load_catalog(root)
            supplement["problems"][0]["replaces_base_revision"] = "base"
            supplement["replaces_support_revision"] = "base"
            supplement["support_overrides"] = {"common/testlib.h": "f" * 64}
            lc.write_json(root / "test/library_checker/supplement_catalog.json", supplement)
            merged = lc.load_catalog(root)
            self.assertEqual(len(merged["problems"]), 1)
            self.assertEqual(lc.suite_revision(merged, merged["problems"][0]), revision)
            self.assertEqual(merged["support_files"]["common/testlib.h"], [original_support, "f" * 64])

    def test_catalog_reads_every_committed_real_problem_not_categories(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            repository = Path(temporary)
            subprocess.run(["git", "init", "-q", str(repository)], check=True)
            (repository / "generate.py").write_text("# official generator\n")
            (repository / "categories.toml").write_text("# no registered problems\n")
            for name in ["uncategorized/actual", "test/internal_fixture"]:
                directory = repository / name
                directory.mkdir(parents=True)
                (directory / "info.toml").write_text("title='X'\ntimelimit=0.5\n[[tests]]\nname='example.in'\nnumber=1\n")
                (directory / "hash.json").write_text("{}")
                (directory / "checker.cpp").write_text("int main() {}")
            subprocess.run(["git", "-C", str(repository), "add", "."], check=True)
            subprocess.run(["git", "-C", str(repository), "-c", "user.name=Test", "-c", "user.email=test@example.invalid", "commit", "-qm", "fixture"], check=True)
            # Local modifications cannot change the committed catalog.
            (repository / "uncategorized/actual/info.toml").write_text("broken local changes")
            result = lc.build_catalog(repository)
            self.assertEqual([entry["id"] for entry in result["problems"]], ["actual"])
            self.assertEqual(result["problems"][0]["timelimit"], 0.5)
            self.assertEqual(len(result["revision"]), 40)

    def test_suite_requires_exact_pairs_and_official_hashes(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            repository = Path(temporary)
            entry = problem()
            add_suite(repository, entry)
            manifest = repository / "proof.json"
            self.assertEqual(lc.validate_suite(repository, entry, manifest, "revision"), 3)
            # An unchanged cache uses a content-provenance manifest.
            with patch.object(lc, "sha256", side_effect=AssertionError("unexpected rehash")):
                self.assertEqual(lc.validate_suite(repository, entry, manifest, "revision"), 3)
            manifest.write_text("incomplete cache write")
            self.assertEqual(lc.validate_suite(repository, entry, manifest, "revision"), 3)
            path = repository / entry["directory"] / "in/example_00.in"
            path.write_text("changed\n")
            with self.assertRaisesRegex(lc.LibraryCheckerError, "hash mismatch"):
                lc.validate_suite(repository, entry, manifest, "revision")

    def test_incomplete_extra_and_changed_expected_cases_fail(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            repository = Path(temporary)
            entry = problem()
            add_suite(repository, entry)
            directory = repository / entry["directory"]
            path = directory / "out/random_01.out"
            path.unlink()
            with self.assertRaisesRegex(lc.LibraryCheckerError, "incomplete out"):
                lc.suite_files(repository, entry)
            path.write_text("random_01\n")
            extra = directory / "in/extra_00.in"
            extra.write_text("extra")
            with self.assertRaisesRegex(lc.LibraryCheckerError, "extra="):
                lc.suite_files(repository, entry)
            extra.unlink()
            hashes = lc.read_json(directory / "hash.json")
            hashes.pop("random_01.out")
            lc.write_json(directory / "hash.json", hashes)
            with self.assertRaisesRegex(lc.LibraryCheckerError, "case set differs"):
                lc.validate_suite(repository, entry, repository / "manifest", "revision")

    def test_pinned_source_change_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            repository = Path(temporary)
            source = repository / "checker.cpp"
            source.write_text("official")
            pinned = {source.name: lc.sha256(source)}
            lc.validate_pinned_files(repository, pinned)
            source.write_text("modified checker")
            with self.assertRaisesRegex(lc.LibraryCheckerError, "differs from pinned"):
                lc.validate_pinned_files(repository, pinned)
            lc.validate_pinned_files(repository, {source.name: [pinned[source.name], lc.sha256(source)]})

    def test_compiler_must_be_actual_gcc_13(self) -> None:
        with patch.object(lc.shutil, "which", return_value="/fake/g++"), patch.object(lc.subprocess, "check_output", side_effect=["14.1\n", "#define __GNUC__ 14\n"]):
            with self.assertRaisesRegex(lc.LibraryCheckerError, "GCC 13 is required"):
                lc.require_gcc13("g++")
        with patch.object(lc.shutil, "which", return_value="/fake/clang++"), patch.object(lc.subprocess, "check_output", side_effect=["13.0\n", "#define __GNUC__ 13\n#define __clang__ 1\n"]):
            with self.assertRaisesRegex(lc.LibraryCheckerError, "GCC 13 is required"):
                lc.require_gcc13("clang++")
        with patch.object(lc.shutil, "which", return_value="/fake/g++-13"), patch.object(lc.subprocess, "check_output", side_effect=["13.4.0\n", "#define __GNUC__ 13\n"]):
            self.assertEqual(lc.require_gcc13("g++-13"), ("/fake/g++-13", "13.4.0"))

    def test_official_unlimited_stack_policy_is_applied(self) -> None:
        unlimited = lc.resource.RLIM_INFINITY
        with patch.object(lc.resource, "getrlimit", return_value=(8388608, unlimited)), patch.object(lc.resource, "setrlimit") as setter:
            lc.require_unlimited_stack()
            setter.assert_called_once_with(lc.resource.RLIMIT_STACK, (unlimited, unlimited))
        with patch.object(lc.resource, "getrlimit", return_value=(8388608, 8388608)):
            with self.assertRaisesRegex(lc.LibraryCheckerError, "unlimited stack"):
                lc.require_unlimited_stack()

    def test_official_generation_uses_gcc13_with_its_cxx17_standard(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            repository = root / ".competitive-verifier/cache/library-checker-problems"
            entry = problem()

            def generate(command: list[str], **kwargs: object) -> subprocess.CompletedProcess:
                self.assertEqual(kwargs["env"]["CXX"], "/gcc13")
                self.assertIn("-std=c++17", kwargs["env"]["CXXFLAGS"])
                self.assertNotIn("-std=c++20", kwargs["env"]["CXXFLAGS"])
                add_suite(repository, entry)
                return subprocess.CompletedProcess(command, 0)

            with patch.object(lc.subprocess, "run", side_effect=generate), patch.object(lc, "compile_checker"):
                self.assertEqual(lc.prepare_problem(root, {"revision": "pin", "support_files": {}}, entry,
                                                   root / "cache", "/gcc13", "13.4.0", True), 3)

    def test_checker_is_compiled_atomically_and_reused_with_provenance(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            repository = Path(temporary)
            entry = problem()
            directory = repository / entry["directory"]
            directory.mkdir(parents=True)
            (directory / "info.toml").write_text("[params]\nN = 7\n")
            (directory / "checker.cpp").write_text("int main() {}\n")
            executable = directory / "checker"
            executable.write_bytes(b"old executable")
            cache = repository / "proofs"

            def fake_compile(command: list[str], **kwargs: object) -> subprocess.CompletedProcess:
                self.assertEqual(executable.read_bytes(), b"old executable")
                output = Path(command[-1])
                self.assertNotEqual(output, executable)
                output.write_bytes(b"new executable")
                output.chmod(0o755)
                return subprocess.CompletedProcess(command, 0)

            with patch.object(lc.subprocess, "run", side_effect=fake_compile):
                lc.compile_checker(repository, entry, cache, "/gcc13", "13.4.0", {})
            self.assertEqual(executable.read_bytes(), b"new executable")
            self.assertEqual((directory / "params.h").read_text(), "#define N (long long)7\n")
            with patch.object(lc.subprocess, "run", side_effect=AssertionError("checker was recompiled")):
                lc.compile_checker(repository, entry, cache, "/gcc13", "13.4.0", {})

    def test_plan_uses_official_limit_and_explicit_compiler_for_untracked_driver(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            driver = add_driver(root)
            header = root / "header.hpp"
            header.write_text("#pragma once\n")
            (root / driver).write_text((root / driver).read_text() + '#include "../../header.hpp"\n')
            plan = lc.make_plan(root, problem(), [driver], root / "cache", "/usr/bin/g++-13")
            verification = plan["files"][driver]["verification"][0]
            self.assertEqual(verification["tle"], 0.125)
            self.assertEqual(verification["compile"][0], "/usr/bin/g++-13")
            self.assertIn("header.hpp", plan["files"][driver]["dependencies"])
            self.assertNotIn("prev-result", str(plan))

    def test_success_requires_all_official_cases_and_limit(self) -> None:
        entry = problem()
        cases = [{"name": name, "status": "AC", "elapsed": 0.01} for name in lc.expected_cases(entry)]
        result = {"files": {"driver.cpp": {"verifications": [{"status": "success", "testcases": cases}]}}}
        self.assertEqual(lc.result_errors(result, entry, ["driver.cpp"]), [])
        cases.pop()
        self.assertTrue(any("testcase set differs" in error for error in lc.result_errors(result, entry, ["driver.cpp"])))
        cases.append({"name": "random_01", "status": "AC", "elapsed": 0.2})
        self.assertTrue(any("exceeded official" in error for error in lc.result_errors(result, entry, ["driver.cpp"])))
        cases[-1]["status"] = "WA"
        self.assertTrue(any("random_01:WA" in error for error in lc.result_errors(result, entry, ["driver.cpp"])))

    def test_verifier_runs_offline_and_without_prior_result(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            driver = add_driver(root)
            cache = root / "cache"
            seen = []

            def fake_run(command: list[str], **kwargs: object) -> subprocess.CompletedProcess:
                seen.extend(command)
                output = Path(command[command.index("--output") + 1])
                cases = [{"name": name, "status": "AC", "elapsed": 0.01} for name in lc.expected_cases(problem())]
                lc.write_json(output, {"files": {driver: {"verifications": [{"status": "success", "testcases": cases}]}}})
                return subprocess.CompletedProcess(command, 0)

            with patch.object(lc.subprocess, "run", side_effect=fake_run):
                _, errors = lc.run_problem(root, problem(), [driver], cache, "g++-13", root / "verifier")
            self.assertEqual(errors, [])
            self.assertIn("--no-download", seen)
            self.assertIn("--check-error", seen)
            self.assertNotIn("--prev-result", seen)

    def test_verified_coverage_rejects_changed_sources_results_and_limits(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            driver = add_driver(root)
            entry = problem()
            catalog = {"revision": "pinned", "problems": [entry]}
            result_path = root / "runs/one/result.json"
            cases = [{"name": name, "status": "AC", "elapsed": 0.01} for name in lc.expected_cases(entry)]
            result = {"files": {driver: {"verifications": [{"status": "success", "testcases": cases}]}}}
            lc.write_json(result_path, result)
            manifest = root / ".competitive-verifier/cache/library-checker/sample/verified.json"
            proof = {"suite_revision": "pinned", "compiler_version": "13.4.0", "timelimit": 0.125,
                     "sources": lc.source_fingerprints(root, [driver]), "result_path": str(result_path),
                     "result_sha256": lc.sha256(result_path)}
            lc.write_json(manifest, proof)
            self.assertEqual(lc.verified_coverage(root, catalog, {"sample": [driver]})["verified"], 1)
            (root / driver).write_text((root / driver).read_text() + "// changed\n")
            self.assertEqual(lc.verified_coverage(root, catalog, {"sample": [driver]})["verified"], 0)
            proof["sources"] = lc.source_fingerprints(root, [driver])
            proof["timelimit"] = 5.0
            lc.write_json(manifest, proof)
            self.assertEqual(lc.verified_coverage(root, catalog, {"sample": [driver]})["verified"], 0)
            proof["timelimit"] = 0.125
            lc.write_json(manifest, proof)
            result_path.write_text("{}")
            self.assertEqual(lc.verified_coverage(root, catalog, {"sample": [driver]})["verified"], 0)

    def test_duplicate_case_names_and_missing_drivers_are_rejected(self) -> None:
        entry = problem()
        entry["tests"].append({"name": "example.cpp", "number": 1})
        with self.assertRaisesRegex(lc.LibraryCheckerError, "invalid case names"):
            lc.expected_cases(entry)
        catalog = {"problems": [problem()]}
        with self.assertRaisesRegex(lc.LibraryCheckerError, "no driver"):
            lc.select_problems(catalog, {}, ["sample"])
        with self.assertRaisesRegex(lc.LibraryCheckerError, "suite not available"):
            lc.select_problems(catalog, {"new": ["driver.cpp"]}, ["new"])


if __name__ == "__main__":
    unittest.main()
