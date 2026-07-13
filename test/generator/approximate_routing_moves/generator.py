#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encoded_route(route: list[int]) -> str:
    return str(len(route)) + "".join(f" {x}" for x in route)


def relocate(route: list[int], first: int, last: int, destination: int) -> list[int]:
    if first == last or first <= destination <= last:
        return route[:]
    block = route[first:last]
    result = route[:first] + route[last:]
    if destination > last:
        destination -= last - first
    result[destination:destination] = block
    return result


def exchange(route: list[int], a: int, b: int, c: int, d: int) -> list[int]:
    if c < a:
        a, b, c, d = c, d, a, b
    return route[:a] + route[c:d] + route[b:c] + route[a:b] + route[d:]


def matrix_text(matrix: list[list[int]]) -> str:
    return " ".join(str(x) for row in matrix for x in row)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071301)
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(45):
        n = rng.randrange(0, 13)
        matrix = [[rng.randrange(-20, 50) for _ in range(n)] for _ in range(n)]
        k = rng.randrange(n + 1)
        route = rng.sample(range(n), k)
        kind = rng.choice(["COST", "PATH"])
        commands.append(f"{kind} {n} {k} {matrix_text(matrix)} {encoded_route(route)[len(str(k)):].strip()}".rstrip())
        if not route:
            value = 0
        elif kind == "COST":
            value = sum(matrix[route[i]][route[(i + 1) % k]] for i in range(k))
        else:
            value = sum(matrix[route[i - 1]][route[i]] for i in range(1, k))
        outputs.append(str(value))

    kinds = ["TWO", "REVERSE", "RELOCATE", "NODE", "OR", "SWAP", "EXCHANGE"]
    for _ in range(310):
        n = rng.randrange(1, 24)
        route = rng.sample(range(1000), n)
        kind = rng.choice(kinds)
        if kind in ("TWO", "REVERSE"):
            first = rng.randrange(n + 1)
            last = rng.randrange(first, n + 1)
            result = route[:first] + list(reversed(route[first:last])) + route[last:]
            suffix = f"{first} {last}"
        elif kind == "RELOCATE":
            first = rng.randrange(n + 1)
            last = rng.randrange(first, n + 1)
            destination = rng.randrange(n + 1)
            result = relocate(route, first, last, destination)
            suffix = f"{first} {last} {destination}"
        elif kind == "NODE":
            position = rng.randrange(n)
            destination = rng.randrange(n + 1)
            result = relocate(route, position, position + 1, destination)
            suffix = f"{position} {destination}"
        elif kind == "OR":
            length = rng.randrange(1, min(3, n) + 1)
            first = rng.randrange(n - length + 1)
            destination = rng.randrange(n + 1)
            result = relocate(route, first, first + length, destination)
            suffix = f"{first} {length} {destination}"
        elif kind == "SWAP":
            first = rng.randrange(n)
            second = rng.randrange(n)
            result = route[:]
            result[first], result[second] = result[second], result[first]
            suffix = f"{first} {second}"
        else:
            cuts = sorted(rng.sample(range(n + 1), 4)) if n >= 3 else [0, 0, n, n]
            a, b, c, d = cuts
            result = exchange(route, a, b, c, d)
            suffix = f"{a} {b} {c} {d}"
        commands.append(f"{kind} {n} {' '.join(map(str, route))} {suffix}")
        outputs.append(encoded_route(result))

    for _ in range(55):
        n = rng.randrange(5, 25)
        route = rng.sample(range(1000), n)
        a, b, c = sorted(rng.sample(range(1, n), 3))
        pattern = rng.randrange(7)
        first = route[a:b]
        second = route[b:c]
        if pattern in (0, 2, 5, 6):
            first.reverse()
        if pattern in (1, 2, 4, 6):
            second.reverse()
        if pattern >= 3:
            first, second = second, first
        result = route[:a] + first + second + route[c:]
        commands.append(
            f"THREE {n} {' '.join(map(str, route))} {a} {b} {c} {pattern}"
        )
        outputs.append(encoded_route(result))

    for _ in range(45):
        n = rng.randrange(2, 18)
        points = [(rng.randrange(-50, 51), rng.randrange(-50, 51)) for _ in range(n)]
        matrix = [
            [abs(x1 - x2) + abs(y1 - y2) for x2, y2 in points]
            for x1, y1 in points
        ]
        route = rng.sample(range(n), n)
        first = rng.randrange(n + 1)
        last = rng.randrange(first, n + 1)
        changed = route[:first] + list(reversed(route[first:last])) + route[last:]
        old = sum(matrix[route[i]][route[(i + 1) % n]] for i in range(n))
        new = sum(matrix[changed[i]][changed[(i + 1) % n]] for i in range(n))
        commands.append(
            f"TWO_DELTA {n} {first} {last} {matrix_text(matrix)} {' '.join(map(str, route))}"
        )
        outputs.append(str(new - old))

    for _ in range(45):
        first_size = rng.randrange(2, 10)
        second_size = rng.randrange(2, 10)
        n = first_size + second_size
        first = list(range(first_size))
        second = list(range(first_size, n))
        rng.shuffle(first)
        rng.shuffle(second)
        first_cut = rng.randrange(1, first_size)
        second_cut = rng.randrange(1, second_size)
        matrix = [[rng.randrange(-30, 80) for _ in range(n)] for _ in range(n)]
        old = sum(matrix[first[i - 1]][first[i]] for i in range(1, first_size))
        old += sum(matrix[second[i - 1]][second[i]] for i in range(1, second_size))
        changed_first = first[:first_cut] + second[second_cut:]
        changed_second = second[:second_cut] + first[first_cut:]
        new = sum(matrix[changed_first[i - 1]][changed_first[i]] for i in range(1, len(changed_first)))
        new += sum(matrix[changed_second[i - 1]][changed_second[i]] for i in range(1, len(changed_second)))
        commands.append(
            f"TWO_STAR_DELTA {n} {first_size} {second_size} "
            f"{first_cut} {second_cut} {matrix_text(matrix)} "
            f"{' '.join(map(str, first + second))}"
        )
        outputs.append(str(new - old))

    for _ in range(40):
        n1 = rng.randrange(0, 15)
        n2 = rng.randrange(0, 15)
        first = rng.sample(range(500), n1)
        second = rng.sample(range(500, 1000), n2)
        cut1 = rng.randrange(n1 + 1)
        cut2 = rng.randrange(n2 + 1)
        result1 = first[:cut1] + second[cut2:]
        result2 = second[:cut2] + first[cut1:]
        commands.append(
            f"TWO_STAR {n1} {n2} {cut1} {cut2} "
            f"{' '.join(map(str, first + second))}"
        )
        outputs.append(f"{encoded_route(result1)} | {encoded_route(result2)}")

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
