#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def matching(n: int, edges: list[tuple[int, int, int]], weighted: bool) -> list[int]:
    if weighted:
        order = [index for index, (u, v, weight) in enumerate(edges) if u != v and weight > 0]
        order.sort(key=lambda index: (-edges[index][2], min(edges[index][:2]), max(edges[index][:2]), index))
    else:
        order = list(range(len(edges)))
    matched = [False] * n
    result = []
    for index in order:
        left, right, _ = edges[index]
        if left != right and not matched[left] and not matched[right]:
            matched[left] = matched[right] = True
            result.append(index)
    return result


def greedy_cut(n: int, edges: list[tuple[int, int, int]]) -> tuple[list[int], int]:
    previous = [[] for _ in range(n)]
    for left, right, _ in edges:
        if left == right:
            continue
        if left < right:
            previous[right].append(left)
        else:
            previous[left].append(right)
    side = [0] * n
    for vertex in range(n):
        cut_zero = sum(side[x] == 1 for x in previous[vertex])
        cut_one = sum(side[x] == 0 for x in previous[vertex])
        if cut_one > cut_zero:
            side[vertex] = 1
    cut = sum(side[left] != side[right] for left, right, _ in edges)
    return side, cut


def encode(values: list[int]) -> str:
    return str(len(values)) + "".join(f" {value}" for value in values)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071313)
    cases: list[tuple[int, list[tuple[int, int, int]]]] = [(0, []), (1, [(0, 0, 9)])]
    for _ in range(50):
        n = rng.randrange(1, 50)
        edges = [
            (rng.randrange(n), rng.randrange(n), rng.randrange(-20, 100))
            for _ in range(rng.randrange(0, n * 5 + 1))
        ]
        cases.append((n, edges))
    n = 1200
    cases.append((n, [(v % n, (17 * v + 19) % n, (v * 1000003) % 1000000007) for v in range(7000)]))

    inputs = [str(len(cases))]
    outputs = []
    for n, edges in cases:
        inputs.append(f"{n} {len(edges)}")
        inputs.extend(f"{left} {right} {weight}" for left, right, weight in edges)
        plain = matching(n, edges, False)
        weighted = matching(n, edges, True)
        side, cut = greedy_cut(n, edges)
        outputs.append(f"{encode(plain)} | {encode(weighted)} | {encode(side)} {cut}")
    (out_dir / "case_00.in").write_text("\n".join(inputs) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
