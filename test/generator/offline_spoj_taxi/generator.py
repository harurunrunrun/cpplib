#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Position = tuple[int, int]


def solve(passengers: list[Position], taxis: list[Position], speed: int, time: int) -> int:
    graph = [[] for _ in passengers]
    for passenger, (px, py) in enumerate(passengers):
        for taxi, (tx, ty) in enumerate(taxis):
            if (abs(px - tx) + abs(py - ty)) * 200 <= speed * time:
                graph[passenger].append(taxi)
    match = [-1] * len(taxis)

    def augment(vertex: int, seen: list[bool]) -> bool:
        for to in graph[vertex]:
            if seen[to]:
                continue
            seen[to] = True
            if match[to] == -1 or augment(match[to], seen):
                match[to] = vertex
                return True
        return False

    return sum(augment(vertex, [False] * len(taxis)) for vertex in range(len(passengers)))


def write_case(out_dir: Path, name: str, cases: list[tuple[list[Position], list[Position], int, int, int | None]]) -> None:
    lines = [str(len(cases))]
    answers = []
    for passengers, taxis, speed, time, answer in cases:
        assert 1 <= len(passengers) <= 400 and 1 <= len(taxis) <= 200
        assert 1 <= speed <= 2000 and 1 <= time <= 1_000_000
        lines.append(f"{len(passengers)} {len(taxis)} {speed} {time}")
        lines.extend(f"{x} {y}" for x, y in passengers)
        lines.extend(f"{x} {y}" for x, y in taxis)
        answers.append(str(solve(passengers, taxis, speed, time) if answer is None else answer))
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(answers) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "official", [([(2, 5), (5, 2)], [(2, 3), (4, 1), (4, 4)], 10, 40, 2)])
    source = random.Random(131)
    random_cases = []
    for _ in range(40):
        passengers = [(source.randint(-8, 8), source.randint(-8, 8)) for _ in range(source.randint(1, 8))]
        taxis = [(source.randint(-8, 8), source.randint(-8, 8)) for _ in range(source.randint(1, 8))]
        random_cases.append((passengers, taxis, source.randint(1, 30), source.randint(1, 100), None))
    write_case(out_dir, "random", random_cases)
    write_case(out_dir, "maximum", [([(0, 0)] * 400, [(0, 0)] * 200, 2000, 1_000_000, 200)])


if __name__ == "__main__":
    main()
