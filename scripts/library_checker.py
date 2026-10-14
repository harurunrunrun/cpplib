#!/usr/bin/env python3
"""Inventory and run official Library Checker suites with GCC 13, without network."""

from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import json
import os
from pathlib import Path
import re
import resource
import shutil
import subprocess
import sys
import tempfile
import time
import tomllib


REPOSITORY_URL = "https://github.com/yosupo06/library-checker-problems"
DEFAULT_ROOT = Path(__file__).resolve().parents[1]
PROBLEM_DIRECTIVE = re.compile(
    r'^\s*(?://\s*competitive-verifier:\s*PROBLEM\s+|#\s*define\s+PROBLEM\s+")'
    r'https?://judge\.yosupo\.jp/problem/([A-Za-z0-9_]+)/?(?:"|\s|$)',
    re.MULTILINE,
)
QUOTED_INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.MULTILINE)
COMPILE_FLAGS = ["-std=c++20", "-O2", "-Wall", "-Wextra"]
GENERATOR_FLAGS = ["-std=c++17", "-O2", "-Wall", "-Wextra", "-Wno-ignored-attributes"]


class LibraryCheckerError(RuntimeError):
    pass


def read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path: Path, value: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(
        mode="w", encoding="utf-8", dir=path.parent, prefix=path.name + ".", delete=False
    ) as temporary:
        temporary.write(json.dumps(value, ensure_ascii=False, indent=2) + "\n")
    Path(temporary.name).replace(path)


def cached_json(path: Path) -> dict | None:
    try:
        return read_json(path)
    except (OSError, ValueError):
        return None


def sha256(path: Path) -> str:
    with path.open("rb") as source:
        return hashlib.file_digest(source, "sha256").hexdigest()


def git_output(repository: Path, *arguments: str) -> bytes:
    return subprocess.check_output(["git", "-C", str(repository), *arguments])


def expected_cases(problem: dict) -> list[str]:
    cases = [
        f"{Path(test['name']).stem}_{index:02d}"
        for test in problem["tests"]
        for index in range(test["number"])
    ]
    if not cases or len(cases) != len(set(cases)):
        raise LibraryCheckerError(f"invalid case names: {problem['id']}")
    return sorted(cases)


def build_catalog(repository: Path) -> dict:
    """Read the committed tree, including problems absent from categories.toml."""
    revision = git_output(repository, "rev-parse", "HEAD").decode().strip()
    paths = git_output(repository, "ls-tree", "-r", "--name-only", revision).decode().splitlines()

    def contents(path: str) -> bytes:
        return git_output(repository, "show", f"{revision}:{path}")

    support = {
        path: hashlib.sha256(contents(path)).hexdigest()
        for path in paths
        if path == "generate.py" or path.startswith("common/")
    }
    problems = []
    for info_path in paths:
        if not info_path.endswith("/info.toml") or info_path.startswith("test/"):
            continue
        info_bytes = contents(info_path)
        info = tomllib.loads(info_bytes.decode())
        directory = Path(info_path).parent
        pinned_files = [info_path, f"{directory}/hash.json", f"{directory}/{info.get('checker', 'checker.cpp')}"]
        pinned_files.extend(
            path for path in paths
            if Path(path).parent == directory and Path(path).suffix in {".h", ".hpp"}
        )
        problem = {
            "id": directory.name,
            "directory": directory.as_posix(),
            "title": info["title"],
            "timelimit": info["timelimit"],
            "tests": info["tests"],
            "checker": info.get("checker", "checker.cpp"),
            "pinned_files": {
                path: hashlib.sha256(contents(path)).hexdigest()
                for path in sorted(set(pinned_files))
            },
        }
        expected_cases(problem)
        problems.append(problem)
    if len({problem["id"] for problem in problems}) != len(problems):
        raise LibraryCheckerError("duplicate problem ids in official tree")
    return {
        "schema_version": 1,
        "repository": REPOSITORY_URL,
        "revision": revision,
        "committed_at": git_output(repository, "show", "-s", "--format=%cI", revision).decode().strip(),
        "support_files": support,
        "problems": sorted(problems, key=lambda problem: problem["id"]),
    }


def build_supplement_catalog(repository: Path, revision: str, problem_ids: list[str]) -> dict:
    """Pin API-downloaded problem directories to an explicitly supplied revision."""
    if re.fullmatch(r"[0-9a-f]{40}", revision) is None:
        raise LibraryCheckerError("supplement revision must be a full Git commit id")
    problems = []
    for problem_id in sorted(set(problem_ids)):
        matches = list(repository.glob(f"**/{problem_id}/info.toml"))
        if len(matches) != 1:
            raise LibraryCheckerError(f"supplement problem not uniquely found: {problem_id}")
        directory = matches[0].parent
        relative = directory.relative_to(repository).as_posix()
        info = tomllib.loads(matches[0].read_text())
        pinned = [matches[0], directory / "hash.json", directory / info.get("checker", "checker.cpp")]
        pinned.extend(path for path in directory.iterdir() if path.suffix in {".h", ".hpp"} and path.name != "params.h")
        support = {}
        pending = [directory / info.get("checker", "checker.cpp")]
        visited = set()
        while pending:
            source = pending.pop()
            if source in visited:
                continue
            visited.add(source)
            for include in QUOTED_INCLUDE.findall(source.read_text()):
                if include == "params.h":
                    continue
                candidates = [source.parent / include, repository / "common" / include]
                target = next((path.resolve() for path in candidates if path.is_file()), None)
                if target is None:
                    raise LibraryCheckerError(f"missing supplement checker include: {include}")
                name = target.relative_to(repository.resolve()).as_posix()
                if name.startswith("common/"):
                    support[name] = sha256(target)
                else:
                    pinned.append(target)
                pending.append(target)
        entry = {
            "id": problem_id, "directory": relative, "title": info["title"],
            "timelimit": info["timelimit"], "tests": info["tests"],
            "checker": info.get("checker", "checker.cpp"),
            "suite_revision": revision, "support_files": support,
            "pinned_files": {path.relative_to(repository).as_posix(): sha256(path) for path in pinned},
        }
        expected_cases(entry)
        problems.append(entry)
    return {"schema_version": 1, "repository": REPOSITORY_URL, "revision": revision, "problems": problems}


def load_catalog(root: Path) -> dict:
    catalog = read_json(root / "test/library_checker/catalog.json")
    supplement_path = root / "test/library_checker/supplement_catalog.json"
    if supplement_path.is_file():
        supplement = read_json(supplement_path)
        if supplement.get("support_overrides"):
            if supplement.get("replaces_support_revision") != catalog["revision"]:
                raise LibraryCheckerError("supplement support revision does not match base")
            for name, digest in supplement["support_overrides"].items():
                previous = catalog["support_files"].get(name)
                if previous is None:
                    raise LibraryCheckerError(f"supplement cannot add unknown common source: {name}")
                # Old suites remain valid with their original common files;
                # fresh installations may need the explicitly pinned new API.
                catalog["support_files"][name] = [previous, digest]
        problems = {problem["id"]: problem for problem in catalog["problems"]}
        for problem in supplement["problems"]:
            if problem["id"] in problems and problem.get("replaces_base_revision") != catalog["revision"]:
                raise LibraryCheckerError(f"supplement duplicates base problem: {problem['id']}")
            if problem.get("suite_revision") != supplement["revision"]:
                raise LibraryCheckerError("supplement per-problem revision differs from its source revision")
            problems[problem["id"]] = problem
        catalog["problems"] = sorted(problems.values(), key=lambda problem: problem["id"])
    return catalog


def suite_revision(catalog: dict, problem: dict) -> str:
    return problem.get("suite_revision", catalog["revision"])


def discover_drivers(root: Path) -> dict[str, list[str]]:
    """Find tracked and untracked test drivers by directive, never by filename."""
    drivers: dict[str, list[str]] = {}
    for path in sorted((root / "test" / "onlinejudge").rglob("*.cpp")):
        ids = set(PROBLEM_DIRECTIVE.findall(path.read_text(encoding="utf-8-sig")))
        if len(ids) > 1:
            raise LibraryCheckerError(f"multiple Library Checker problems: {path}")
        for problem_id in ids:
            drivers.setdefault(problem_id, []).append(path.relative_to(root).as_posix())
    return drivers


def coverage_report(catalog: dict, inventory: dict, drivers: dict[str, list[str]]) -> dict:
    known = {problem["id"] for problem in inventory["problems"]}
    available = {problem["id"] for problem in catalog["problems"]}
    unknown = sorted(set(drivers) - known)
    if unknown:
        raise LibraryCheckerError("drivers absent from pinned inventory: " + ", ".join(unknown))
    return {
        "inventory_revision": inventory["revision"],
        "suite_revision": catalog["revision"],
        "suite_revisions": sorted({suite_revision(catalog, problem) for problem in catalog["problems"]}),
        "total": len(known),
        "covered": len(known & drivers.keys()),
        "driver_count": sum(map(len, drivers.values())),
        "drivers": dict(sorted(drivers.items())),
        "missing": sorted(known - drivers.keys()),
        "unavailable_suites": sorted(known - available),
    }


def require_gcc13(compiler: str) -> tuple[str, str]:
    executable = shutil.which(compiler)
    if executable is None:
        raise LibraryCheckerError(f"GCC 13 compiler not found: {compiler}")
    version = subprocess.check_output(
        [executable, "-dumpfullversion", "-dumpversion"], text=True
    ).strip()
    macros = subprocess.check_output(
        [executable, "-dM", "-E", "-x", "c++", "-"], input="", text=True
    )
    if version.split(".", 1)[0] != "13" or "#define __clang__ " in macros:
        raise LibraryCheckerError(f"GCC 13 is required; {executable} reports {version}")
    if not re.search(r"^#define __GNUC__ 13$", macros, re.MULTILINE):
        raise LibraryCheckerError(f"GCC 13 is required: {executable}")
    return str(Path(executable).resolve()), version


def require_unlimited_stack() -> None:
    """Use the stack policy required by the official generator's README."""
    soft, hard = resource.getrlimit(resource.RLIMIT_STACK)
    if hard != resource.RLIM_INFINITY:
        raise LibraryCheckerError("official suites require unlimited stack; run ulimit -s unlimited")
    if soft != resource.RLIM_INFINITY:
        resource.setrlimit(resource.RLIMIT_STACK, (resource.RLIM_INFINITY, hard))


def validate_pinned_files(repository: Path, hashes: dict) -> None:
    for name, digest in hashes.items():
        path = repository / name
        allowed = digest if isinstance(digest, list) else [digest]
        if not path.is_file() or sha256(path) not in allowed:
            raise LibraryCheckerError(f"official source differs from pinned revision: {path}")


def suite_files(repository: Path, problem: dict) -> dict[str, Path]:
    directory = repository / problem["directory"]
    cases = expected_cases(problem)
    paths = {}
    for subdir, suffix in [("in", ".in"), ("out", ".out")]:
        actual = {path.stem for path in (directory / subdir).glob(f"*{suffix}")}
        if actual != set(cases):
            missing = sorted(set(cases) - actual)
            extra = sorted(actual - set(cases))
            raise LibraryCheckerError(
                f"{problem['id']}: incomplete {subdir} suite; "
                f"missing={missing[:5]}, extra={extra[:5]}"
            )
        for case in cases:
            paths[case + suffix] = directory / subdir / (case + suffix)
    return paths


def file_stamp(path: Path) -> list[int]:
    stat = path.stat()
    return [stat.st_size, stat.st_mtime_ns, stat.st_ctime_ns]


def validate_suite(repository: Path, problem: dict, manifest: Path, revision: str) -> int:
    """Check official content hashes; reuse only an unchanged validated file set."""
    paths = suite_files(repository, problem)
    official = read_json(repository / problem["directory"] / "hash.json")
    if paths.keys() != official.keys():
        raise LibraryCheckerError(f"{problem['id']}: hash.json case set differs from info.toml")
    proof = {
        "revision": revision,
        "official_hashes": official,
        "files": {name: file_stamp(path) for name, path in paths.items()},
    }
    if cached_json(manifest) == proof:
        return len(paths) // 2
    for name, path in paths.items():
        if sha256(path) != official[name]:
            raise LibraryCheckerError(f"{problem['id']}: official testcase hash mismatch: {path}")
    write_json(manifest, proof)
    return len(paths) // 2


def params_header(info: dict) -> str:
    lines = []
    for key, value in info.get("params", {}).items():
        if isinstance(value, int):
            value = f"(long long){value}"
        elif isinstance(value, str):
            value = f'"{value}"'
        lines.append(f"#define {key} {value}\n")
    return "".join(lines)


def compile_checker(
    repository: Path, problem: dict, cache: Path, compiler: str, version: str, support: dict
) -> None:
    directory = repository / problem["directory"]
    info = tomllib.loads((directory / "info.toml").read_text())
    header = directory / "params.h"
    content = params_header(info)
    if not header.is_file() or header.read_text() != content:
        header.write_text(content)
    executable = directory / Path(problem["checker"]).with_suffix("")
    # competitive-verifier's LibraryCheckerProblem uses this exact path.
    if executable != directory / "checker":
        raise LibraryCheckerError(f"unsupported official checker location: {executable}")
    proof = {
        "compiler": compiler,
        "version": version,
        "flags": COMPILE_FLAGS,
        "support_files": support,
        "problem_sources": problem["pinned_files"],
        "params_sha256": sha256(header),
    }
    manifest = cache / "checker.json"
    previous = cached_json(manifest)
    if previous is not None and executable.is_file():
        if previous.get("source") == proof and previous.get("binary") == sha256(executable):
            return
    cache.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=cache, prefix="checker-build-") as temporary:
        compiled = Path(temporary) / "checker"
        with (cache / "checker.log").open("w") as log:
            result = subprocess.run(
                [compiler, *COMPILE_FLAGS, "-Wno-ignored-attributes", "-I", str(repository / "common"),
                 str(directory / problem["checker"]), "-o", str(compiled)],
                stdout=log, stderr=subprocess.STDOUT, check=False,
            )
        if result.returncode:
            raise LibraryCheckerError(f"{problem['id']}: checker compilation failed; see {cache / 'checker.log'}")
        compiled.replace(executable)
    write_json(manifest, {"source": proof, "binary": sha256(executable)})


def prepare_problem(
    root: Path, catalog: dict, problem: dict, cache: Path, compiler: str, version: str,
    generate: bool,
) -> int:
    repository = root / ".competitive-verifier/cache/library-checker-problems"
    validate_pinned_files(repository, problem["pinned_files"])
    validate_pinned_files(repository, problem.get("support_files", {}))
    try:
        suite_files(repository, problem)
    except LibraryCheckerError:
        if not generate:
            raise
        cache.mkdir(parents=True, exist_ok=True)
        environment = os.environ.copy()
        environment["CXX"] = compiler
        # Official sources require C++17: some use names such as `integral`,
        # which collide with C++20 concepts. Library drivers remain C++20.
        environment["CXXFLAGS"] = " ".join(GENERATOR_FLAGS)
        with (cache / "generate.log").open("w") as log:
            result = subprocess.run(
                [sys.executable, str(repository / "generate.py"),
                 str(repository / problem["directory"] / "info.toml")],
                cwd=root, env=environment, stdout=log, stderr=subprocess.STDOUT, check=False,
            )
        if result.returncode:
            raise LibraryCheckerError(f"{problem['id']}: official generation failed; see {cache / 'generate.log'}")
    count = validate_suite(repository, problem, cache / "suite.json", suite_revision(catalog, problem))
    support = problem.get("support_files", catalog["support_files"])
    # Record the actual common-source contents used, not a set of alternatives.
    actual_support = {name: sha256(repository / name) for name in support}
    compile_checker(repository, problem, cache, compiler, version, actual_support)
    return count


def dependencies(root: Path, driver: str) -> list[str]:
    visited: set[Path] = set()

    def visit(path: Path) -> None:
        path = path.resolve()
        if path in visited:
            return
        if not path.is_relative_to(root) or not path.is_file():
            raise LibraryCheckerError(f"include outside repository or missing: {path}")
        visited.add(path)
        for include in QUOTED_INCLUDE.findall(path.read_text(encoding="utf-8-sig")):
            candidates = [path.parent / include, root / include]
            target = next((candidate for candidate in candidates if candidate.is_file()), None)
            if target is None:
                raise LibraryCheckerError(f"unresolved include {include!r} in {path}")
            visit(target)

    visit(root / driver)
    return sorted(path.relative_to(root).as_posix() for path in visited)


def source_fingerprints(root: Path, drivers: list[str]) -> dict:
    return {
        driver: {name: sha256(root / name) for name in dependencies(root, driver)}
        for driver in drivers
    }


def verified_coverage(root: Path, catalog: dict, drivers: dict[str, list[str]]) -> dict:
    """Count only recorded full-suite successes whose sources still match."""
    verified = {}
    for problem in catalog["problems"]:
        problem_id = problem["id"]
        manifest = root / ".competitive-verifier/cache/library-checker" / problem_id / "verified.json"
        if problem_id not in drivers or not manifest.is_file():
            continue
        try:
            proof = read_json(manifest)
            if proof["suite_revision"] != suite_revision(catalog, problem) or proof["timelimit"] != problem["timelimit"]:
                continue
            if proof["compiler_version"].split(".", 1)[0] != "13":
                continue
            result_path = Path(proof["result_path"])
            if sha256(result_path) != proof["result_sha256"]:
                continue
            proof_drivers = sorted(proof["sources"])
            if result_errors(read_json(result_path), problem, proof_drivers):
                continue
            matches = []
            for driver in proof_drivers:
                if driver in drivers[problem_id] and source_fingerprints(root, [driver])[driver] == proof["sources"][driver]:
                    matches.append(driver)
            if matches:
                verified[problem_id] = matches
        except (OSError, ValueError, KeyError, LibraryCheckerError):
            continue
    return {"verified": len(verified), "verified_driver_count": sum(map(len, verified.values())),
            "verified_drivers": verified}


def make_plan(root: Path, problem: dict, drivers: list[str], cache: Path, compiler: str) -> dict:
    files = {}
    for driver in drivers:
        key = hashlib.sha256(driver.encode()).hexdigest()[:16]
        executable = (cache / f"driver-{key}").as_posix()
        files[driver] = {
            "dependencies": dependencies(root, driver),
            "verification": [{
                "name": "GCC 13 / official timelimit",
                "type": "problem",
                "problem": f"https://judge.yosupo.jp/problem/{problem['id']}",
                "command": [executable],
                "compile": [compiler, *COMPILE_FLAGS, "-I", str(root), driver, "-o", executable],
                "tle": problem["timelimit"],
            }],
        }
    return {"files": files}


def result_errors(result: dict, problem: dict, drivers: list[str]) -> list[str]:
    errors = []
    files = result.get("files", {})
    if set(files) != set(drivers):
        errors.append("result driver set differs from plan")
    expected = expected_cases(problem)
    for driver in drivers:
        verifications = files.get(driver, {}).get("verifications", [])
        if len(verifications) != 1:
            errors.append(f"{driver}: expected one verification")
            continue
        verification = verifications[0]
        cases = verification.get("testcases") or []
        if sorted(case["name"] for case in cases) != expected:
            errors.append(f"{driver}: result testcase set differs from official suite")
        failures = [f"{case['name']}:{case['status']}" for case in cases if case["status"] != "AC"]
        if verification.get("status") != "success" or failures:
            errors.append(f"{driver}: {verification.get('status')}; " + ", ".join(failures[:8]))
        if any(case["elapsed"] > problem["timelimit"] for case in cases):
            errors.append(f"{driver}: exceeded official {problem['timelimit']} second limit")
    return errors


def run_problem(
    root: Path, problem: dict, drivers: list[str], cache: Path, compiler: str,
    verifier: Path,
) -> tuple[dict, list[str]]:
    plan = make_plan(root, problem, drivers, cache, compiler)
    plan_path = cache / "verify-files.json"
    result_path = cache / "result.json"
    write_json(plan_path, plan)
    result_path.unlink(missing_ok=True)
    environment = os.environ.copy()
    environment["COMPETITIVE_VERIFY_CONFIG_PATH"] = str(root / ".competitive-verifier")
    with (cache / "verify.log").open("w") as log:
        completed = subprocess.run(
            [str(verifier), "verify", "--verify-json", str(plan_path), "--no-download",
             "--check-error", "--output", str(result_path)],
            cwd=root, env=environment, stdout=log, stderr=subprocess.STDOUT, check=False,
        )
    if not result_path.is_file():
        return {}, [f"verifier exited {completed.returncode} without result; see {cache / 'verify.log'}"]
    result = read_json(result_path)
    errors = result_errors(result, problem, drivers)
    if completed.returncode:
        errors.append(f"verifier exited {completed.returncode}; see {cache / 'verify.log'}")
    return result, errors


def select_problems(catalog: dict, drivers: dict, selected: list[str]) -> list[dict]:
    problems = {problem["id"]: problem for problem in catalog["problems"]}
    ids = sorted(set(selected) if selected else set(drivers))
    for problem_id in ids:
        if problem_id not in problems:
            raise LibraryCheckerError(f"official suite not available at pinned revision: {problem_id}")
        if problem_id not in drivers:
            raise LibraryCheckerError(f"no driver: {problem_id}")
    if not ids:
        raise LibraryCheckerError("no Library Checker drivers found")
    return [problems[problem_id] for problem_id in ids]


def run_batch(args: argparse.Namespace, root: Path, catalog: dict, drivers: dict) -> int:
    compiler, version = require_gcc13(args.cxx)
    require_unlimited_stack()
    repository = root / ".competitive-verifier/cache/library-checker-problems"
    revision = git_output(repository, "rev-parse", "HEAD").decode().strip()
    if revision != catalog["revision"]:
        raise LibraryCheckerError(f"suite revision differs: expected {catalog['revision']}, found {revision}")
    validate_pinned_files(repository, catalog["support_files"])
    selected = select_problems(catalog, drivers, args.problem)
    cache = root / ".competitive-verifier/cache/library-checker"
    output_dir = (args.output_dir.resolve() if args.output_dir else
                  cache / "runs" / f"{args.command}-{time.time_ns()}-{os.getpid()}")
    verifier = root / ".competitive-verifier/venv/bin/competitive-verifier"
    if args.command == "verify" and not verifier.is_file():
        raise LibraryCheckerError("competitive-verifier is missing; run make verifier-setup")
    started = time.monotonic()
    report = {
        "suite_revision": revision, "compiler": compiler, "compiler_version": version,
        "flags": COMPILE_FLAGS, "timelimits": "official info.toml", "results": {},
        "stack_limit": "unlimited (official generator policy)",
        "selected_problems": [problem["id"] for problem in selected],
        "suite_revisions": {problem["id"]: suite_revision(catalog, problem) for problem in selected},
    }
    merged = {"total_seconds": 0.0, "files": {}}

    def worker(problem: dict) -> tuple[str, dict, dict]:
        problem_id = problem["id"]
        destination = cache / problem_id
        try:
            count = prepare_problem(root, catalog, problem, destination, compiler, version, not args.no_generate)
            entry = {"status": "prepared", "case_count": count, "drivers": drivers[problem_id],
                     "timelimit": problem["timelimit"], "suite_revision": suite_revision(catalog, problem)}
            result = {}
            if args.command == "verify":
                sources = source_fingerprints(root, drivers[problem_id])
                proof_path = destination / "verified.json"
                proof_path.unlink(missing_ok=True)
                result_destination = output_dir / problem_id
                result, errors = run_problem(root, problem, drivers[problem_id], result_destination, compiler, verifier)
                if source_fingerprints(root, drivers[problem_id]) != sources:
                    errors.append("driver or library sources changed during verification; rerun")
                entry["status"] = "failure" if errors else "success"
                entry["result_directory"] = result_destination.as_posix()
                if errors:
                    entry["errors"] = errors
                else:
                    result_path = result_destination / "result.json"
                    write_json(proof_path, {
                        "suite_revision": suite_revision(catalog, problem), "compiler_version": version,
                        "timelimit": problem["timelimit"], "sources": sources,
                        "result_path": result_path.as_posix(), "result_sha256": sha256(result_path),
                    })
            return problem_id, entry, result
        except (OSError, ValueError, LibraryCheckerError, subprocess.SubprocessError) as error:
            return problem_id, {"status": "failure", "errors": [str(error)]}, {}

    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = [executor.submit(worker, problem) for problem in selected]
        for future in as_completed(futures):
            problem_id, entry, result = future.result()
            report["results"][problem_id] = entry
            merged["files"].update(result.get("files", {}))
            merged["total_seconds"] += result.get("total_seconds", 0.0)
            report["elapsed_seconds"] = time.monotonic() - started
            write_json(output_dir / f"{args.command}-report.json", report)
            if args.command == "verify":
                write_json(output_dir / "result.json", merged)
            print(f"[library-checker] {problem_id}: {entry['status']}", flush=True)
            for error in entry.get("errors", []):
                print(f"  {error}", file=sys.stderr, flush=True)
    failures = sum(entry["status"] == "failure" for entry in report["results"].values())
    print(f"[library-checker] {len(selected) - failures}/{len(selected)} {args.command} succeeded; "
          f"suite revisions {', '.join(sorted(set(report['suite_revisions'].values())))}; "
          f"report {output_dir / (args.command + '-report.json')}")
    return int(failures != 0)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=DEFAULT_ROOT)
    subparsers = parser.add_subparsers(dest="command", required=True)
    catalog_parser = subparsers.add_parser("catalog", help="pin all real problems from the cached committed tree")
    catalog_parser.add_argument("--output", type=Path)
    supplement_parser = subparsers.add_parser("supplement-catalog", help="pin separately downloaded official problem sources")
    supplement_parser.add_argument("--repository", type=Path, required=True)
    supplement_parser.add_argument("--revision", required=True)
    supplement_parser.add_argument("--problem", action="append", required=True)
    supplement_parser.add_argument("--output", type=Path)
    supplement_parser.add_argument("--replace-base-problem", action="append", default=[],
                                   help="explicitly replace metadata for a base-catalog problem")
    supplement_parser.add_argument("--allow-support-update", action="append", default=[],
                                   help="allow this exact newer common source alongside the original")
    coverage_parser = subparsers.add_parser("coverage", help="report drivers against the latest pinned inventory")
    coverage_parser.add_argument("--require-complete", action="store_true")
    coverage_parser.add_argument("--json", type=Path)
    for name in ("prepare", "verify"):
        command_parser = subparsers.add_parser(name)
        command_parser.add_argument("--problem", action="append", default=[], help="repeat to select problems; default: all covered")
        command_parser.add_argument("--jobs", type=int, default=4)
        command_parser.add_argument("--cxx", default="g++-13")
        command_parser.add_argument("--no-generate", action="store_true", help="require already cached complete suites")
        command_parser.add_argument("--output-dir", type=Path, help="report/log directory; default: a separate directory per invocation")
    args = parser.parse_args(argv)
    root = args.root.resolve()
    try:
        catalog_path = root / "test/library_checker/catalog.json"
        if args.command == "catalog":
            value = build_catalog(root / ".competitive-verifier/cache/library-checker-problems")
            write_json(args.output or catalog_path, value)
            print(f"pinned {len(value['problems'])} problems at {value['revision']}")
            return 0
        if args.command == "supplement-catalog":
            value = build_supplement_catalog(args.repository.resolve(), args.revision, args.problem)
            base_revision = read_json(catalog_path)["revision"]
            replacements = set(args.replace_base_problem)
            for problem in value["problems"]:
                if problem["id"] in replacements:
                    problem["replaces_base_revision"] = base_revision
                    replacements.remove(problem["id"])
            if replacements:
                raise LibraryCheckerError("replacement not selected: " + ", ".join(sorted(replacements)))
            if args.allow_support_update:
                value["replaces_support_revision"] = base_revision
                value["support_overrides"] = {}
                for name in args.allow_support_update:
                    if not name.startswith("common/") or ".." in Path(name).parts:
                        raise LibraryCheckerError(f"invalid common support path: {name}")
                    value["support_overrides"][name] = sha256(args.repository.resolve() / name)
            destination = args.output or root / "test/library_checker/supplement_catalog.json"
            write_json(destination, value)
            print(f"pinned {len(value['problems'])} supplement problems at {value['revision']}")
            return 0
        catalog = load_catalog(root)
        drivers = discover_drivers(root)
        if args.command == "coverage":
            inventory_path = root / "test/library_checker/latest_inventory.json"
            inventory = read_json(inventory_path) if inventory_path.is_file() else catalog
            report = coverage_report(catalog, inventory, drivers)
            report.update(verified_coverage(root, catalog, drivers))
            if args.json:
                write_json(args.json, report)
            print(f"Library Checker: {report['covered']}/{report['total']} problems have drivers "
                  f"({report['driver_count']} drivers); {len(report['missing'])} missing")
            print(f"official full-suite successes with current sources: {report['verified']} problems "
                  f"({report['verified_driver_count']} drivers)")
            print(f"inventory revision: {report['inventory_revision']}; suite revision: {report['suite_revision']}")
            if len(report["suite_revisions"]) > 1:
                print("supplemented suite revisions: " + ", ".join(report["suite_revisions"]))
            if report["missing"]:
                print("missing: " + ", ".join(report["missing"]))
            if report["unavailable_suites"]:
                print("not in cached suite revision: " + ", ".join(report["unavailable_suites"]))
            return int(args.require_complete and bool(report["missing"]))
        if args.jobs <= 0:
            parser.error("--jobs must be positive")
        return run_batch(args, root, catalog, drivers)
    except (OSError, ValueError, LibraryCheckerError, subprocess.SubprocessError) as error:
        print(f"[library-checker] {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
