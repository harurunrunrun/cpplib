#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import shlex
import subprocess
import sys
from collections import Counter
from pathlib import Path


def read_input(path):
    tokens = path.read_text(encoding="utf-8").split()
    if len(tokens) < 2:
        raise ValueError("input header is missing")
    n, m = map(int, tokens[:2])
    if len(tokens) != 2 + n * m:
        raise ValueError("input token count is invalid")
    values = list(map(int, tokens[2:]))
    return n, m, [values[row * m:(row + 1) * m] for row in range(n)]


def validate_output(data, n, m, input_rows):
    try:
        tokens = data.decode("utf-8").split()
    except UnicodeDecodeError as error:
        return f"output is not UTF-8: {error}"
    if len(tokens) != 1 + n * m:
        return f"expected {1 + n * m} tokens, got {len(tokens)}"
    if tokens[0] != "Yes":
        return "first token must be Yes"
    try:
        values = list(map(int, tokens[1:]))
    except ValueError:
        return "matrix contains a non-integer token"
    rows = [values[row * m:(row + 1) * m] for row in range(n)]
    for row in range(n):
        if Counter(rows[row]) != Counter(input_rows[row]):
            return f"row {row + 1} does not preserve its input multiset"
    required = list(range(1, n + 1))
    for column in range(m):
        if sorted(rows[row][column] for row in range(n)) != required:
            return f"column {column + 1} is not a permutation of 1..N"
    return None


def main():
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
    executable = build_dir / "a.out"
    compile_result = subprocess.run(
        [args.cxx, *shlex.split(args.cxxflags), "-I", ".", str(test), "-o", str(executable)],
        check=False,
    )
    if compile_result.returncode != 0:
        print(f"[offline-atcoder-abc317-g] FAIL {test}: compilation failed", file=sys.stderr)
        return 1

    failures = []
    input_files = sorted(case_dir.glob("*.in"))
    if not input_files:
        failures.append(("cases", "no input file"))
    input_stems = {path.stem for path in input_files}
    for output_file in sorted(case_dir.glob("*.out")):
        if output_file.stem not in input_stems:
            failures.append((output_file.name, "orphan expected output"))

    for input_file in input_files:
        expected_file = input_file.with_suffix(".out")
        if not expected_file.exists():
            failures.append((input_file.name, "missing expected output"))
            continue
        try:
            n, m, input_rows = read_input(input_file)
        except Exception as error:
            failures.append((input_file.name, f"invalid generated input: {error}"))
            continue

        expected_error = validate_output(expected_file.read_bytes(), n, m, input_rows)
        if expected_error is not None:
            failures.append((expected_file.name, f"invalid generated answer: {expected_error}"))
            continue

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
            failures.append((input_file.name, f"timed out after {args.timeout:g} seconds"))
            continue
        if result.returncode != 0:
            detail = f"exited with code {result.returncode}"
            stderr = result.stderr.decode(errors="replace").strip()
            if stderr:
                detail += f"; stderr: {stderr[:4096]}"
            failures.append((input_file.name, detail))
            continue
        actual_error = validate_output(result.stdout, n, m, input_rows)
        if actual_error is not None:
            failures.append((input_file.name, actual_error))

    if failures:
        print(
            f"[offline-atcoder-abc317-g] FAIL {test}: {len(failures)} failure(s)",
            file=sys.stderr,
        )
        for label, message in failures:
            print(f"  - {label}: {message}", file=sys.stderr)
        return 1
    print(
        f"[offline-atcoder-abc317-g] PASS {test}: {len(input_files)} case(s)",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
