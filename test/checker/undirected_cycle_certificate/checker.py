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
    parser.add_argument("--test", required=True, type=Path)
    parser.add_argument("--case-dir", required=True, type=Path)
    parser.add_argument("--build-dir", required=True, type=Path)
    parser.add_argument("--cxx", default="g++")
    parser.add_argument("--cxxflags", default="-std=c++20 -O2 -Wall -Wextra")
    parser.add_argument("--timeout", type=float, default=30.0)
    args = parser.parse_args()

    args.build_dir.mkdir(parents=True, exist_ok=True)
    executable = args.build_dir / "a.out"
    compile_result = subprocess.run(
        [
            args.cxx,
            *shlex.split(args.cxxflags),
            "-I",
            ".",
            str(args.test),
            "-o",
            str(executable),
        ],
        check=False,
    )
    if compile_result.returncode != 0:
        print("cycle certificate checker: compilation failed", file=sys.stderr)
        return 1

    failures: list[str] = []
    for input_path in sorted(args.case_dir.glob("*.in")):
        values = list(map(int, input_path.read_text(encoding="ascii").split()))
        vertex_count, edge_count = values[:2]
        edges = [
            (values[2 + 2 * index], values[3 + 2 * index])
            for index in range(edge_count)
        ]
        expected = int(input_path.with_suffix(".out").read_text(encoding="ascii"))
        try:
            process = subprocess.run(
                [str(executable)],
                input=input_path.read_bytes(),
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=args.timeout,
                check=False,
            )
        except subprocess.TimeoutExpired:
            failures.append(f"{input_path.name}: timeout")
            continue
        if process.returncode != 0:
            failures.append(f"{input_path.name}: exit {process.returncode}")
            continue
        try:
            output = list(map(int, process.stdout.split()))
        except ValueError:
            failures.append(f"{input_path.name}: non-integer output")
            continue
        if not output:
            failures.append(f"{input_path.name}: empty output")
            continue
        if output[0] == -1:
            if len(output) != 1 or expected != 0:
                failures.append(f"{input_path.name}: incorrect no-cycle result")
            continue

        length = output[0]
        if expected != 1 or length <= 0 or len(output) != 1 + 2 * length:
            failures.append(f"{input_path.name}: invalid certificate shape")
            continue
        vertices = output[1 : 1 + length]
        edge_ids = output[1 + length :]
        if any(vertex < 0 or vertex_count <= vertex for vertex in vertices):
            failures.append(f"{input_path.name}: vertex out of range")
            continue
        if len(set(vertices)) != length or len(set(edge_ids)) != length:
            failures.append(f"{input_path.name}: repeated vertex or edge")
            continue
        valid = True
        for index, edge_id in enumerate(edge_ids):
            if edge_id < 0 or edge_count <= edge_id:
                valid = False
                break
            left, right = edges[edge_id]
            first = vertices[index]
            second = vertices[(index + 1) % length]
            if not ((left == first and right == second) or
                    (left == second and right == first)):
                valid = False
                break
        if not valid:
            failures.append(f"{input_path.name}: invalid cycle certificate")

    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    print(
        f"cycle certificate checker: {len(list(args.case_dir.glob('*.in')))} case(s) passed",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
