#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import shlex
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path

from standalone_verification_results import (
    make_manifest,
    result_path,
    source_fingerprint,
    write_manifest,
)


def escape_workflow_command(value: str) -> str:
    """Escape a GitHub Actions workflow-command property or message."""
    return (
        value.replace("%", "%25")
        .replace("\r", "%0D")
        .replace("\n", "%0A")
        .replace(":", "%3A")
        .replace(",", "%2C")
    )


def report_failure_annotation(name: str, message: str) -> None:
    title = escape_workflow_command(f"standalone-assets: {name}")
    escaped_message = escape_workflow_command(message)
    print(
        f"::error title={title}::{escaped_message}",
        file=sys.stderr,
    )


def asset_command(
    asset: Path,
    native_executable: Path,
    cxx: str,
    cxxflags: str,
) -> list[str]:
    if asset.suffix == ".py":
        return [sys.executable, str(asset)]
    native_executable.parent.mkdir(parents=True, exist_ok=True)
    compile_result = subprocess.run(
        [
            cxx,
            *shlex.split(cxxflags),
            "-I",
            ".",
            str(asset),
            "-o",
            str(native_executable),
        ],
        check=False,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(
            f"native asset compilation exited with code {compile_result.returncode}"
        )
    return [str(native_executable)]


def run_with_failure_diagnostic(command: list[str]) -> tuple[int, str]:
    """Run COMMAND without retaining successful output; return failure tail."""

    with tempfile.TemporaryFile() as output:
        completed = subprocess.run(
            command,
            check=False,
            stdout=output,
            stderr=output,
        )
        if completed.returncode == 0:
            return 0, ""
        size = output.tell()
        output.seek(max(0, size - 4096))
        diagnostic = output.read().decode("utf-8", errors="replace").strip()
        return completed.returncode, diagnostic


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--cache-dir", default=".competitive-verifier/cache/standalone-assets")
    parser.add_argument(
        "--result-dir",
        default=".competitive-verifier/cache/standalone-results",
    )
    parser.add_argument("--cxx", default="g++")
    parser.add_argument("--cxxflags", default="-std=c++20 -O2 -Wall -Wextra")
    parser.add_argument("--split-size", type=int, default=1)
    parser.add_argument("--split-index", type=int, default=0)
    args = parser.parse_args(argv)

    if args.split_size <= 0:
        parser.error("--split-size must be positive")
    if not 0 <= args.split_index < args.split_size:
        parser.error("--split-index must be in [0, --split-size)")

    root = Path(".")
    cache_dir = Path(args.cache_dir)
    result_dir = Path(args.result_dir)
    try:
        fingerprint = source_fingerprint(root)
    except Exception as error:
        message = f"could not fingerprint standalone sources: {error}"
        print(f"[standalone-assets] {message}", file=sys.stderr)
        report_failure_annotation("fingerprint", message)
        return 1
    all_tests = sorted((root / "test" / "standalone").glob("*.test.cpp"))
    tests = all_tests[args.split_index :: args.split_size]
    failures: list[tuple[str, str]] = []

    if not all_tests:
        failures.append(("discovery", "no standalone tests found"))

    print(
        "[standalone-assets] "
        f"shard {args.split_index}/{args.split_size}: "
        f"{len(tests)}/{len(all_tests)} test(s)",
        file=sys.stderr,
    )

    for test in tests:
        name = test.name[: -len(".test.cpp")]
        test_path = test.as_posix()
        manifest_path = result_path(result_dir, test_path)
        started = time.monotonic()
        print(f"[standalone-assets] START {name}", file=sys.stderr)
        try:
            write_manifest(
                manifest_path,
                make_manifest(
                    test_path=test_path,
                    fingerprint=fingerprint,
                    status="running",
                    cxx=args.cxx,
                    cxxflags=args.cxxflags,
                ),
            )
        except Exception as error:
            failures.append((name, f"could not record running result: {error}"))
        generator_python = root / "test" / "generator" / name / "generator.py"
        generator_cpp = root / "test" / "generator" / name / "generator.cpp"
        checker_python = root / "test" / "checker" / name / "checker.py"
        checker_cpp = root / "test" / "checker" / name / "checker.cpp"
        generator = generator_python if generator_python.exists() else generator_cpp
        checker = checker_python if checker_python.exists() else checker_cpp
        generator_exists = generator_python.exists() or generator_cpp.exists()
        checker_exists = checker_python.exists() or checker_cpp.exists()
        if not generator_exists:
            failures.append((name, f"missing generator: {generator}"))
        if not checker_exists:
            failures.append((name, f"missing checker: {checker}"))

        case_dir = cache_dir / name / "cases"
        try:
            if case_dir.exists():
                shutil.rmtree(case_dir)
            case_dir.mkdir(parents=True, exist_ok=True)
        except Exception as error:
            failures.append((name, f"could not prepare case directory: {error}"))
            try:
                write_manifest(
                    manifest_path,
                    make_manifest(
                        test_path=test_path,
                        fingerprint=fingerprint,
                        status="failure",
                        elapsed_seconds=time.monotonic() - started,
                        case_count=0,
                        cxx=args.cxx,
                        cxxflags=args.cxxflags,
                    ),
                )
            except Exception as manifest_error:
                failures.append(
                    (name, f"could not record failure result: {manifest_error}")
                )
            continue

        build_dir = cache_dir / name / "build"

        generator_succeeded = False
        if generator_exists:
            try:
                generator_command = [
                    *asset_command(
                        generator, build_dir / "generator.out", args.cxx, args.cxxflags
                    ),
                    "--out-dir",
                    str(case_dir),
                ]
                generator_returncode, generator_diagnostic = (
                    run_with_failure_diagnostic(generator_command)
                )
            except Exception as error:
                failures.append((name, f"generator could not start: {error}"))
            else:
                if generator_returncode == 0:
                    generator_succeeded = True
                else:
                    diagnostic = (
                        f"; diagnostic:\n{generator_diagnostic}"
                        if generator_diagnostic
                        else ""
                    )
                    failures.append(
                        (
                            name,
                            "generator exited with code "
                            f"{generator_returncode}; command: "
                            f"{shlex.join(generator_command)}{diagnostic}",
                        )
                    )

        case_count = len(list(case_dir.glob("*.in")))
        if generator_succeeded and case_count == 0:
            failures.append(
                (name, "generator produced no .in cases; checker was skipped")
            )

        if checker_exists and generator_succeeded and case_count > 0:
            try:
                checker_result = subprocess.run(
                    [
                        *asset_command(
                            checker, build_dir / "checker.out", args.cxx, args.cxxflags
                        ),
                        "--test",
                        str(test),
                        "--case-dir",
                        str(case_dir),
                        "--build-dir",
                        str(build_dir),
                        "--cxx",
                        args.cxx,
                        "--cxxflags",
                        args.cxxflags,
                    ],
                    check=False,
                )
            except Exception as error:
                failures.append((name, f"checker could not start: {error}"))
            else:
                if checker_result.returncode != 0:
                    failures.append(
                        (
                            name,
                            f"checker exited with code {checker_result.returncode}",
                        )
                    )

        successful = not any(failed_name == name for failed_name, _ in failures)
        try:
            write_manifest(
                manifest_path,
                make_manifest(
                    test_path=test_path,
                    fingerprint=fingerprint,
                    status="success" if successful else "failure",
                    elapsed_seconds=time.monotonic() - started,
                    case_count=case_count,
                    cxx=args.cxx,
                    cxxflags=args.cxxflags,
                ),
            )
        except Exception as error:
            failures.append((name, f"could not record final result: {error}"))
            successful = False

        if successful:
            print(f"[standalone-assets] PASS {name}", file=sys.stderr)

    if failures:
        print(
            f"[standalone-assets] FAIL: {len(failures)} failure(s)",
            file=sys.stderr,
        )
        for name, message in failures:
            print(f"  - {name}: {message}", file=sys.stderr)
            report_failure_annotation(name, message)
        return 1

    print(
        f"[standalone-assets] PASS: {len(tests)} test(s)",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
