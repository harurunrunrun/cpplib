#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260729)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)

    cases: list[tuple[int, list[tuple[int, int]], tuple[int, int, int, int]]] = [
        (2, [(0, 1)], (0, 0, 1, 0)),
        (2, [(0, 1), (0, 1), (0, 1)], (0, 1, 3, 0)),
        (3, [(0, 1), (1, 2), (2, 0)], (1, 0, 3, 0)),
        (
            4,
            [(u, v) for u in range(4) for v in range(u + 1, 4)],
            (0, 0, 6, 1),
        ),
        (
            5,
            [(0, 2), (2, 1), (0, 3), (3, 1), (0, 4), (4, 1)],
            (3, 1, 6, 0),
        ),
        (
            4,
            [(0, 1), (1, 2), (2, 3), (3, 0), (0, 2)],
            (-1, -1, -1, -1),
        ),
    ]
    for _ in range(240):
        n = rng.randint(3, 10)
        order = list(range(n))
        rng.shuffle(order)
        edges = [
            (order[index], order[(index + 1) % n])
            for index in range(n)
        ]
        for u in range(n):
            for v in range(u + 1, n):
                if (u, v) in edges or (v, u) in edges:
                    continue
                if rng.random() < 0.24:
                    edges.append((u, v))
        for _ in range(rng.randint(0, 3)):
            edges.append(rng.choice(edges))
        rng.shuffle(edges)
        cases.append((n, edges, (-1, -1, -1, -1)))

    lines = [str(len(cases))]
    for n, edges, expected in cases:
        lines.append(
            f"{n} {len(edges)} " + " ".join(map(str, expected))
        )
        lines.extend(f"{u} {v}" for u, v in edges)
    (args.out_dir / "cases.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "ok\n" * len(cases), encoding="utf-8"
    )


if __name__ == "__main__":
    main()
