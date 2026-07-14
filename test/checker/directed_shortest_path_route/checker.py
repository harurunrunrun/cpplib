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
        print("shortest path route checker: compilation failed", file=sys.stderr)
        return 1

    failures: list[str] = []
    for input_path in sorted(args.case_dir.glob("*.in")):
        values = list(map(int, input_path.read_text(encoding="ascii").split()))
        _, edge_count, source, target = values[:4]
        edges = [
            (
                values[4 + 3 * index],
                values[5 + 3 * index],
                values[6 + 3 * index],
            )
            for index in range(edge_count)
        ]
        expected_distance = int(
            input_path.with_suffix(".out").read_text(encoding="ascii")
        )
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
        if output == [-1]:
            if expected_distance != -1:
                failures.append(f"{input_path.name}: reachable target reported unreachable")
            continue
        if len(output) < 2:
            failures.append(f"{input_path.name}: missing route header")
            continue
        distance, length = output[:2]
        if length < 0 or len(output) != 2 + 2 * length:
            failures.append(f"{input_path.name}: invalid route shape")
            continue
        if expected_distance == -1 or distance != expected_distance:
            failures.append(f"{input_path.name}: incorrect shortest distance")
            continue

        minimum_cost: dict[tuple[int, int], int] = {}
        for from_vertex, to_vertex, cost in edges:
            key = (from_vertex, to_vertex)
            minimum_cost[key] = min(minimum_cost.get(key, cost), cost)
        current = source
        route_cost = 0
        valid = True
        for index in range(length):
            from_vertex = output[2 + 2 * index]
            to_vertex = output[3 + 2 * index]
            if from_vertex != current or (from_vertex, to_vertex) not in minimum_cost:
                valid = False
                break
            route_cost += minimum_cost[(from_vertex, to_vertex)]
            current = to_vertex
        if not valid or current != target or route_cost != distance:
            failures.append(f"{input_path.name}: invalid shortest route")

    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    print(
        f"shortest path route checker: {len(list(args.case_dir.glob('*.in')))} case(s) passed",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
