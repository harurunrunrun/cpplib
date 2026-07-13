#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import shlex
import shutil
import subprocess
import sys
from pathlib import Path


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


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--cache-dir", default=".competitive-verifier/cache/standalone-assets")
    parser.add_argument("--cxx", default="g++")
    parser.add_argument("--cxxflags", default="-std=c++20 -O2 -Wall -Wextra")
    args = parser.parse_args()

    root = Path(".")
    cache_dir = Path(args.cache_dir)
    tests = sorted((root / "test" / "standalone").glob("*.test.cpp"))
    failures: list[tuple[str, str]] = []

    if not tests:
        failures.append(("discovery", "no standalone tests found"))

    for test in tests:
        name = test.name[: -len(".test.cpp")]
        print(f"[standalone-assets] START {name}", file=sys.stderr)
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
            continue

        build_dir = cache_dir / name / "build"

        if generator_exists:
            try:
                generator_result = subprocess.run(
                    [
                        *asset_command(
                            generator, build_dir / "generator.out", args.cxx, args.cxxflags
                        ),
                        "--out-dir",
                        str(case_dir),
                    ],
                    check=False,
                )
            except Exception as error:
                failures.append((name, f"generator could not start: {error}"))
            else:
                if generator_result.returncode != 0:
                    failures.append(
                        (
                            name,
                            f"generator exited with code {generator_result.returncode}",
                        )
                    )

        if checker_exists:
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

        if not any(failed_name == name for failed_name, _ in failures):
            print(f"[standalone-assets] PASS {name}", file=sys.stderr)

    if failures:
        print(
            f"[standalone-assets] FAIL: {len(failures)} failure(s)",
            file=sys.stderr,
        )
        for name, message in failures:
            print(f"  - {name}: {message}", file=sys.stderr)
        return 1

    print(
        f"[standalone-assets] PASS: {len(tests)} test(s)",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
