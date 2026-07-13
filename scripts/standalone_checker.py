#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import shlex
import subprocess
import sys
from pathlib import Path


def preview(data: bytes, limit: int = 4096) -> str:
    if len(data) <= limit:
        return repr(data)
    return f"{data[:limit]!r}... ({len(data)} bytes total)"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--test", required=True)
    parser.add_argument("--case-dir", required=True)
    parser.add_argument("--build-dir", required=True)
    parser.add_argument("--cxx", default="g++")
    parser.add_argument("--cxxflags", default="-std=c++20 -O2 -Wall -Wextra")
    parser.add_argument("--timeout", type=float, default=30.0)
    args = parser.parse_args()

    test = Path(args.test)
    case_dir = Path(args.case_dir)
    build_dir = Path(args.build_dir)
    build_dir.mkdir(parents=True, exist_ok=True)
    exe = build_dir / "a.out"

    compile_command = [
        args.cxx,
        *shlex.split(args.cxxflags),
        "-I",
        ".",
        str(test),
        "-o",
        str(exe),
    ]
    failures: list[tuple[str, str]] = []

    try:
        compile_result = subprocess.run(compile_command, check=False)
    except Exception as error:
        failures.append(("compile", f"could not start compiler: {error}"))
    else:
        if compile_result.returncode != 0:
            failures.append(
                ("compile", f"compiler exited with code {compile_result.returncode}")
            )

    if failures:
        print(
            f"[standalone-checker] FAIL {test}: compilation failed; cases were not run",
            file=sys.stderr,
        )
        for label, message in failures:
            print(f"  - {label}: {message}", file=sys.stderr)
        return 1

    input_files = sorted(case_dir.glob("*.in"))
    if not input_files:
        failures.append(("cases", f"no input file: {case_dir}"))

    input_stems = {path.stem for path in input_files}
    orphan_outputs = sorted(
        path.name for path in case_dir.glob("*.out") if path.stem not in input_stems
    )
    for output in orphan_outputs:
        failures.append((output, "orphan expected output without matching .in"))

    for input_file in input_files:
        expected_file = input_file.with_suffix(".out")
        if not expected_file.exists():
            failures.append((input_file.name, f"missing expected output: {expected_file}"))

        try:
            result = subprocess.run(
                [str(exe)],
                input=input_file.read_bytes(),
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=args.timeout,
            )
        except subprocess.TimeoutExpired:
            failures.append(
                (
                    input_file.name,
                    f"timed out after {args.timeout:g} seconds",
                )
            )
            continue
        except Exception as error:
            failures.append((input_file.name, f"could not execute case: {error}"))
            continue

        if result.returncode != 0:
            stderr = result.stderr.decode(errors="replace").strip()
            detail = f"exited with code {result.returncode}"
            if stderr:
                detail += f"; stderr: {stderr[:4096]}"
            failures.append((input_file.name, detail))
            continue

        if not expected_file.exists():
            continue
        try:
            expected = expected_file.read_bytes()
        except Exception as error:
            failures.append(
                (input_file.name, f"could not read expected output: {error}")
            )
            continue

        if result.stdout != expected:
            failures.append(
                (
                    input_file.name,
                    "output mismatch; "
                    f"expected: {preview(expected)}; "
                    f"actual: {preview(result.stdout)}",
                )
            )

    if failures:
        print(
            f"[standalone-checker] FAIL {test}: {len(failures)} failure(s)",
            file=sys.stderr,
        )
        for label, message in failures:
            print(f"  - {label}: {message}", file=sys.stderr)
        return 1

    print(
        f"[standalone-checker] PASS {test}: {len(input_files)} case(s)",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
