#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path


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
        generator = root / "test" / "generator" / name / "generator.py"
        checker = root / "test" / "checker" / name / "checker.py"
        generator_exists = generator.exists()
        checker_exists = checker.exists()
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
                    [sys.executable, str(generator), "--out-dir", str(case_dir)],
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
                        sys.executable,
                        str(checker),
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
