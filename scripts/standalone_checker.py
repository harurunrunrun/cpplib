#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import shlex
import subprocess
from pathlib import Path


def main() -> None:
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
    subprocess.run(compile_command, check=True)

    input_files = sorted(case_dir.glob("*.in"))
    if not input_files:
        raise RuntimeError(f"no input file: {case_dir}")

    input_stems = {path.stem for path in input_files}
    orphan_outputs = sorted(
        path.name for path in case_dir.glob("*.out") if path.stem not in input_stems
    )
    if orphan_outputs:
        raise RuntimeError(f"orphan expected output: {', '.join(orphan_outputs)}")

    for input_file in input_files:
        expected_file = input_file.with_suffix(".out")
        if not expected_file.exists():
            raise RuntimeError(f"missing expected output: {expected_file}")

        try:
            result = subprocess.run(
                [str(exe)],
                input=input_file.read_bytes(),
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=args.timeout,
            )
        except subprocess.TimeoutExpired as error:
            raise RuntimeError(
                f"{test} timed out on {input_file.name} after {args.timeout:g} seconds"
            ) from error
        if result.returncode != 0:
            raise RuntimeError(
                f"{test} failed on {input_file.name} with exit code {result.returncode}\n"
                + result.stderr.decode(errors="replace")
            )

        expected = expected_file.read_bytes()
        if result.stdout != expected:
            raise RuntimeError(
                f"{test} output mismatch on {input_file.name}\n"
                f"expected: {expected!r}\n"
                f"actual:   {result.stdout!r}"
            )


if __name__ == "__main__":
    main()
