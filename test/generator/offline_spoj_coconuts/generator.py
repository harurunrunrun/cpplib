#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve(beliefs: list[int], friendships: list[tuple[int, int]]) -> int:
    n = len(beliefs)
    return min(
        sum(((mask >> vertex) & 1) != beliefs[vertex] for vertex in range(n))
        + sum(((mask >> left) & 1) != ((mask >> right) & 1) for left, right in friendships)
        for mask in range(1 << n)
    )


def write_case(out_dir: Path, name: str, cases: list[tuple[list[int], list[tuple[int, int]], int | None]]) -> None:
    lines: list[str] = []
    answers: list[str] = []
    for beliefs, friendships, answer in cases:
        n = len(beliefs)
        assert 2 <= n <= 300 and 1 <= len(friendships) <= n * (n - 1) // 2
        lines.extend([f"{n} {len(friendships)}", " ".join(map(str, beliefs))])
        lines.extend(f"{left + 1} {right + 1}" for left, right in friendships)
        answers.append(str(solve(beliefs, friendships) if answer is None else answer))
    lines.append("0 0")
    (out_dir / f"{name}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(answers) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "official", [
        ([1, 0, 0], [(0, 1), (0, 2), (2, 1)], 1),
        ([1, 1, 1, 0, 0, 0], [(0, 1), (1, 2), (3, 1), (2, 4), (3, 4), (4, 5)], 2),
    ])
    source = random.Random(128)
    random_cases = []
    for _ in range(35):
        n = source.randint(2, 9)
        possible = [(a, b) for a in range(n) for b in range(a + 1, n)]
        friendships = source.sample(possible, source.randint(1, len(possible)))
        random_cases.append(([source.randrange(2) for _ in range(n)], friendships, None))
    write_case(out_dir, "random", random_cases)
    n = 300
    complete = [(a, b) for a in range(n) for b in range(a + 1, n)]
    write_case(out_dir, "maximum", [([0] * 150 + [1] * 150, complete, 150)])


if __name__ == "__main__":
    main()
