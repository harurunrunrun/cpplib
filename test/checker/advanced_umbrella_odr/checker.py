#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import shlex
import subprocess
import sys
from pathlib import Path


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
    build_dir = Path(args.build_dir)
    build_dir.mkdir(parents=True, exist_ok=True)
    executable = build_dir / "a.out"
    sources = [
        test,
        test.parent / "advanced_umbrella_odr_part_a.cpp",
        test.parent / "advanced_umbrella_odr_part_b.cpp",
    ]
    compile_result = subprocess.run([
        args.cxx,
        *shlex.split(args.cxxflags),
        "-I",
        ".",
        *(str(source) for source in sources),
        "-o",
        str(executable),
    ], check=False)
    if compile_result.returncode != 0:
        print("[advanced-umbrella-checker] FAIL: compilation", file=sys.stderr)
        return 1

    input_files = sorted(Path(args.case_dir).glob("*.in"))
    if not input_files:
        print("[advanced-umbrella-checker] FAIL: no cases", file=sys.stderr)
        return 1
    for input_file in input_files:
        expected_file = input_file.with_suffix(".out")
        if not expected_file.exists():
            print(f"[advanced-umbrella-checker] FAIL: missing {expected_file}", file=sys.stderr)
            return 1
        try:
            result = subprocess.run(
                [str(executable)],
                input=input_file.read_bytes(),
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=args.timeout,
                check=False,
            )
        except subprocess.TimeoutExpired:
            print(f"[advanced-umbrella-checker] FAIL: timeout {input_file}", file=sys.stderr)
            return 1
        if result.returncode != 0 or result.stdout != expected_file.read_bytes():
            print(f"[advanced-umbrella-checker] FAIL: {input_file}", file=sys.stderr)
            if result.stderr:
                print(result.stderr.decode(errors="replace"), file=sys.stderr)
            return 1
    print(
        f"[advanced-umbrella-checker] PASS {test}: {len(input_files)} case(s)",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
