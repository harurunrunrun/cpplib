#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path

INF = 10**30


class Case:
    def __init__(self, size: int, source: int) -> None:
        self.size = size
        self.source = source
        self.commands: list[str] = []
        self.graph: list[list[tuple[int, int]]] = [[] for _ in range(size)]

    def edge(self, source: int, destination: int, weight: int) -> None:
        self.commands.append(f"E {source} {destination} {weight}")
        self.graph[source].append((destination, weight))

    def point_range(
        self, source: int, left: int, right: int, weight: int
    ) -> None:
        self.commands.append(f"PR {source} {left} {right} {weight}")
        for destination in range(left, right):
            self.graph[source].append((destination, weight))

    def range_point(
        self, left: int, right: int, destination: int, weight: int
    ) -> None:
        self.commands.append(f"RP {left} {right} {destination} {weight}")
        for source in range(left, right):
            self.graph[source].append((destination, weight))

    def range_range(
        self,
        source_left: int,
        source_right: int,
        destination_left: int,
        destination_right: int,
        weight: int,
    ) -> None:
        self.commands.append(
            f"RR {source_left} {source_right} "
            f"{destination_left} {destination_right} {weight}"
        )
        for source in range(source_left, source_right):
            for destination in range(destination_left, destination_right):
                self.graph[source].append((destination, weight))

    def distances(self) -> list[int]:
        distance = [INF] * self.size
        distance[self.source] = 0
        queue = [(0, self.source)]
        while queue:
            current, vertex = heapq.heappop(queue)
            if current != distance[vertex]:
                continue
            for destination, weight in self.graph[vertex]:
                following = current + weight
                if following < distance[destination]:
                    distance[destination] = following
                    heapq.heappush(queue, (following, destination))
        return [-1 if value == INF else value for value in distance]


def write_case(out_dir: Path, name: str, case: Case) -> None:
    (out_dir / f"{name}.in").write_text(
        "\n".join(
            [
                f"{case.size} {len(case.commands)}",
                *case.commands,
                str(case.source),
            ]
        )
        + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        " ".join(map(str, case.distances())) + "\n",
        encoding="utf-8",
    )


def boundary_case() -> Case:
    case = Case(1, 0)
    case.point_range(0, 0, 0, 7)
    case.range_point(0, 0, 0, 8)
    case.range_range(0, 0, 0, 1, 9)
    case.range_range(0, 1, 1, 1, 10)
    case.edge(0, 0, 11)
    case.point_range(0, 0, 1, 12)
    case.range_point(0, 1, 0, 13)
    case.range_range(0, 1, 0, 1, 14)
    return case


def random_case() -> Case:
    rng = random.Random(2026071605)
    size = 35
    case = Case(size, 17)
    for _ in range(1500):
        operation = rng.randrange(4)
        weight = rng.randrange(30)
        first, second = sorted((rng.randrange(size + 1), rng.randrange(size + 1)))
        if operation == 0:
            case.edge(rng.randrange(size), rng.randrange(size), weight)
        elif operation == 1:
            case.point_range(rng.randrange(size), first, second, weight)
        elif operation == 2:
            case.range_point(first, second, rng.randrange(size), weight)
        else:
            third, fourth = sorted(
                (rng.randrange(size + 1), rng.randrange(size + 1))
            )
            case.range_range(first, second, third, fourth, weight)
    return case


def large_case() -> Case:
    size = 20000
    case = Case(size, 0)
    for vertex in range(size - 1):
        case.edge(vertex, vertex + 1, 1)
    for vertex in range(0, size, 4):
        case.point_range(
            vertex,
            vertex + 1,
            min(size, vertex + 6),
            2,
        )
    for vertex in range(5, size, 5):
        case.range_point(max(0, vertex - 5), vertex, vertex, 3)
    for vertex in range(0, size, 7):
        case.range_range(
            vertex,
            min(size, vertex + 2),
            min(size, vertex + 2),
            min(size, vertex + 5),
            4,
        )
    return case


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "case_00_boundary", boundary_case())
    write_case(out_dir, "case_01_random", random_case())
    write_case(out_dir, "case_02_large", large_case())


if __name__ == "__main__":
    main()
