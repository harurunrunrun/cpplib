#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def cost(distance: list[list[int]], tour: list[int]) -> int:
    if not tour:
        return 0
    return sum(distance[tour[i]][tour[(i + 1) % len(tour)]] for i in range(len(tour)))


def cheapest_position(
    distance: list[list[int]], tour: list[int], vertex: int
) -> tuple[int, int]:
    choices = []
    for position, current in enumerate(tour):
        previous = tour[position - 1]
        delta = distance[previous][vertex] + distance[vertex][current] - distance[previous][current]
        choices.append((delta, position))
    delta, position = min(choices)
    return position, delta


def nearest_neighbor(distance: list[list[int]], start: int) -> list[int]:
    n = len(distance)
    if not n:
        return []
    used = [False] * n
    used[start] = True
    tour = [start]
    current = start
    while len(tour) < n:
        vertex = min((distance[current][v], v) for v in range(n) if not used[v])[1]
        used[vertex] = True
        tour.append(vertex)
        current = vertex
    return tour


def insertion(distance: list[list[int]], start: int, kind: str, regret_k: int = 2) -> list[int]:
    n = len(distance)
    if not n:
        return []
    tour = [start]
    used = [False] * n
    used[start] = True
    nearest = distance[start][:]
    while len(tour) < n:
        remaining = [v for v in range(n) if not used[v]]
        if kind == "NEAREST":
            vertex = min((nearest[v], v) for v in remaining)[1]
            position, _ = cheapest_position(distance, tour, vertex)
        elif kind == "FARTHEST":
            vertex = min(remaining, key=lambda v: (-nearest[v], v))
            position, _ = cheapest_position(distance, tour, vertex)
        elif kind == "CHEAPEST":
            delta, vertex, position = min(
                (cheapest_position(distance, tour, v)[1], v, cheapest_position(distance, tour, v)[0])
                for v in remaining
            )
            del delta
        else:
            candidates = []
            for v in remaining:
                choices = sorted(
                    (
                        distance[tour[p - 1]][v]
                        + distance[v][tour[p]]
                        - distance[tour[p - 1]][tour[p]],
                        p,
                    )
                    for p in range(len(tour))
                )
                kth = min(regret_k, len(choices)) - 1
                regret = choices[kth][0] - choices[0][0]
                candidates.append((-regret, choices[0][0], v, choices[0][1]))
            _, _, vertex, position = min(candidates)
        tour.insert(position, vertex)
        used[vertex] = True
        for v in remaining:
            if not used[v]:
                nearest[v] = min(nearest[v], distance[vertex][v])
    return tour


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071302)
    commands: list[str] = []
    outputs: list[str] = []

    specifications: list[tuple[str, int, int, int, list[list[int]]]] = []
    specifications.extend((kind, 0, 0, 2, []) for kind in ["NN", "RNN", "NEAREST", "CHEAPEST", "FARTHEST", "REGRET"])
    for _ in range(135):
        n = rng.randrange(1, 15)
        points = [(rng.randrange(-100, 101), rng.randrange(-100, 101)) for _ in range(n)]
        if rng.randrange(2):
            distance = [
                [abs(x1 - x2) + abs(y1 - y2) for x2, y2 in points]
                for x1, y1 in points
            ]
        else:
            distance = [[0 if i == j else rng.randrange(1, 500) for j in range(n)] for i in range(n)]
        kind = rng.choice(["NN", "RNN", "NEAREST", "CHEAPEST", "FARTHEST", "REGRET"])
        start = rng.randrange(n)
        regret_k = rng.randrange(1, 6)
        specifications.append((kind, n, start, regret_k, distance))

    n = 85
    distance = [[abs(i - j) + ((i * 17 + j * 31) % 7) for j in range(n)] for i in range(n)]
    for i in range(n):
        distance[i][i] = 0
    specifications.extend([
        ("NN", n, 37, 2, distance),
        ("NEAREST", n, 0, 2, distance),
        ("FARTHEST", n, 0, 2, distance),
        ("CHEAPEST", n, 0, 2, distance),
        ("REGRET", n, 0, 3, distance),
    ])

    for kind, n, start, regret_k, distance in specifications:
        flat = " ".join(str(value) for row in distance for value in row)
        if kind == "RNN":
            command = f"RNN {n} {flat}".rstrip()
            if n:
                tour = min(
                    (nearest_neighbor(distance, s) for s in range(n)),
                    key=lambda value: cost(distance, value),
                )
            else:
                tour = []
        elif kind == "NN":
            command = f"NN {n} {start} {flat}".rstrip()
            tour = nearest_neighbor(distance, start)
        elif kind == "REGRET":
            command = f"REGRET {n} {start} {regret_k} {flat}".rstrip()
            tour = insertion(distance, start, kind, regret_k)
        else:
            command = f"{kind} {n} {start} {flat}".rstrip()
            tour = insertion(distance, start, kind)
        commands.append(command)
        outputs.append(
            str(len(tour))
            + "".join(f" {vertex}" for vertex in tour)
            + f" | {cost(distance, tour)}"
        )

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
