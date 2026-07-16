#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path

PERMUTATIONS = {
    "RF": (3, 0, 2, 5, 4, 1),
    "RB": (1, 5, 2, 0, 4, 3),
    "RR": (4, 1, 0, 3, 5, 2),
    "RL": (2, 1, 5, 3, 0, 4),
    "SR": (0, 2, 3, 4, 1, 5),
    "SL": (0, 4, 1, 2, 3, 5),
}
OPPOSITE = (5, 3, 4, 1, 2, 0)
VALID_PAIRS = tuple(
    (top, front)
    for top in range(6)
    for front in range(6)
    if top != front and OPPOSITE[top] != front
)


def apply(state: tuple[int, ...], permutation: tuple[int, ...]) -> tuple[int, ...]:
    return tuple(state[index] for index in permutation)


def orientations(state: tuple[int, ...]) -> list[tuple[int, ...]]:
    answer = [state]
    queue = deque([state])
    while queue:
        current = queue.popleft()
        for operation in ("RF", "RR", "SR"):
            following = apply(current, PERMUTATIONS[operation])
            if following not in answer:
                answer.append(following)
                queue.append(following)
    assert len(answer) == 24
    return answer


def write_case(
    out_dir: Path,
    name: str,
    initial: tuple[int, ...],
    commands: list[str],
    output: list[str],
) -> None:
    (out_dir / f"{name}.in").write_text(
        "\n".join(
            [str(len(commands)), " ".join(map(str, initial)), *commands]
        )
        + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output) + ("\n" if output else ""),
        encoding="utf-8",
    )


def boundary_case() -> tuple[
    tuple[int, ...],
    list[str],
    list[str],
]:
    initial = (1, 2, 3, 4, 5, 6)
    state = initial
    commands = ["STATE"]
    output = [" ".join(map(str, state))]

    for operation, permutation in PERMUTATIONS.items():
        commands.extend((operation, "STATE"))
        state = apply(state, permutation)
        output.append(" ".join(map(str, state)))

    for top, front in VALID_PAIRS:
        commands.append(f"ORIENT {top} {front}")
        target_top = state[top]
        target_front = state[front]
        state = next(
            candidate
            for candidate in orientations(state)
            if candidate[0] == target_top and candidate[1] == target_front
        )
        commands.append("STATE")
        output.append(" ".join(map(str, state)))

    for candidate in orientations(state):
        commands.append("EQUIV " + " ".join(map(str, candidate)))
        output.append("1")
    commands.append("EQUIV 101 102 103 104 105 106")
    output.append("0")
    return initial, commands, output


def random_case(
    seed: int,
    operation_count: int,
) -> tuple[tuple[int, ...], list[str], list[str]]:
    rng = random.Random(seed)
    initial = (11, 22, 33, 44, 55, 66)
    state = initial
    commands: list[str] = []
    output: list[str] = []
    operation_names = tuple(PERMUTATIONS)

    for _ in range(operation_count):
        kind = rng.randrange(10)
        if kind < 6:
            operation = operation_names[kind]
            commands.append(operation)
            state = apply(state, PERMUTATIONS[operation])
        elif kind == 6:
            commands.append("STATE")
            output.append(" ".join(map(str, state)))
        elif kind == 7:
            top, front = rng.choice(VALID_PAIRS)
            commands.append(f"ORIENT {top} {front}")
            target_top = state[top]
            target_front = state[front]
            state = next(
                candidate
                for candidate in orientations(state)
                if candidate[0] == target_top and candidate[1] == target_front
            )
        elif kind == 8:
            other = rng.choice(orientations(state))
            commands.append("EQUIV " + " ".join(map(str, other)))
            output.append("1")
        else:
            other = tuple(rng.sample(range(101, 107), 6))
            commands.append("EQUIV " + " ".join(map(str, other)))
            output.append("0")

    return initial, commands, output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "case_00_boundary", *boundary_case())
    write_case(out_dir, "case_01_random", *random_case(2026071601, 6000))
    write_case(out_dir, "case_02_large", *random_case(2026071606, 50000))


if __name__ == "__main__":
    main()
