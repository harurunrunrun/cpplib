#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Dfa = tuple[list[list[int]], list[int], int]
Minimized = tuple[list[list[int]], list[int], int, list[int]]


def naive_classes(
    transition: list[list[int]],
    accepting: list[int],
    active: list[bool],
) -> list[int]:
    classes = [
        int(accepting[state]) if active[state] else -1
        for state in range(len(transition))
    ]
    while True:
        ids: dict[tuple[int, ...], int] = {}
        next_classes = [-1] * len(transition)
        for state, row in enumerate(transition):
            if not active[state]:
                continue
            signature = (int(accepting[state]), *(classes[to] for to in row))
            if signature not in ids:
                ids[signature] = len(ids)
            next_classes[state] = ids[signature]
        if next_classes == classes:
            return classes
        classes = next_classes


def naive_minimize(
    transition: list[list[int]],
    accepting: list[int],
    initial: int,
) -> Minimized:
    n = len(transition)
    active = [False] * n
    active[initial] = True
    stack = [initial]
    while stack:
        state = stack.pop()
        for to in transition[state]:
            if not active[to]:
                active[to] = True
                stack.append(to)

    classes = naive_classes(transition, accepting, active)
    class_count = max(classes) + 1
    representative = [-1] * class_count
    for state in range(n):
        if active[state] and representative[classes[state]] == -1:
            representative[classes[state]] = state

    initial_class = classes[initial]
    class_to_minimized = [-1] * class_count
    class_to_minimized[initial_class] = 0
    order = [initial_class]
    minimized_transition: list[list[int]] = []
    minimized_accepting: list[int] = []
    for class_id in order:
        state = representative[class_id]
        row: list[int] = []
        for to in transition[state]:
            to_class = classes[to]
            if class_to_minimized[to_class] == -1:
                class_to_minimized[to_class] = len(order)
                order.append(to_class)
            row.append(class_to_minimized[to_class])
        minimized_transition.append(row)
        minimized_accepting.append(int(accepting[state]))

    mapping = [-1] * n
    for state in range(n):
        if active[state]:
            mapping[state] = class_to_minimized[classes[state]]
    return minimized_transition, minimized_accepting, 0, mapping


def input_text(dfa: Dfa) -> str:
    transition, accepting, initial = dfa
    alphabet_size = len(transition[0])
    lines = [
        f"{len(transition)} {alphabet_size} {initial}",
        " ".join(map(str, accepting)),
    ]
    lines.extend(" ".join(map(str, row)) for row in transition)
    return "\n".join(lines) + "\n"


def output_text(result: Minimized) -> str:
    transition, accepting, initial, mapping = result
    alphabet_size = len(transition[0])
    lines = [
        f"{len(transition)} {alphabet_size} {initial}",
        " ".join(map(str, accepting)),
        " ".join(map(str, mapping)),
    ]
    lines.extend(" ".join(map(str, row)) for row in transition)
    return "\n".join(lines) + "\n"


def write_case(
    out_dir: Path,
    name: str,
    dfa: Dfa,
    expected: Minimized | None = None,
) -> None:
    if expected is None:
        expected = naive_minimize(*dfa)
    (out_dir / f"{name}.in").write_text(
        input_text(dfa),
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        output_text(expected),
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "single_reject", ([[0]], [0], 0))
    write_case(out_dir, "single_accept", ([[0, 0]], [1], 0))
    write_case(out_dir, "empty_alphabet", ([[], [], []], [1, 0, 1], 1))
    write_case(
        out_dir,
        "unreachable",
        (
            [
                [1, 2],
                [1, 3],
                [1, 3],
                [3, 3],
                [5, 4],
                [4, 5],
            ],
            [0, 0, 0, 1, 0, 1],
            0,
        ),
    )
    write_case(
        out_dir,
        "all_equivalent",
        (
            [
                [1, 2, 0],
                [2, 0, 1],
                [0, 1, 2],
            ],
            [0, 0, 0],
            0,
        ),
    )

    for seed in range(14):
        rng = random.Random(20260802 + seed)
        n = rng.randint(1, 32)
        alphabet_size = rng.randint(0, 5)
        transition = [
            [rng.randrange(n) for _ in range(alphabet_size)]
            for _ in range(n)
        ]
        accepting = [rng.randrange(2) for _ in range(n)]
        initial = rng.randrange(n)
        write_case(
            out_dir,
            f"random_{seed:02d}",
            (transition, accepting, initial),
        )

    chain_n = 8000
    chain_transition = [
        [min(state + 1, chain_n - 1)]
        for state in range(chain_n)
    ]
    chain_accepting = [0] * chain_n
    chain_accepting[-1] = 1
    chain_expected: Minimized = (
        chain_transition,
        chain_accepting,
        0,
        list(range(chain_n)),
    )
    write_case(
        out_dir,
        "large_unary_chain",
        (chain_transition, chain_accepting, 0),
        chain_expected,
    )

    equivalent_n = 25000
    equivalent_transition = [
        [
            (state + 1) % equivalent_n,
            (state * 17 + 3) % equivalent_n,
            state,
        ]
        for state in range(equivalent_n)
    ]
    equivalent_accepting = [0] * equivalent_n
    equivalent_expected: Minimized = (
        [[0, 0, 0]],
        [0],
        0,
        [0] * equivalent_n,
    )
    write_case(
        out_dir,
        "large_all_equivalent",
        (equivalent_transition, equivalent_accepting, 137),
        equivalent_expected,
    )


if __name__ == "__main__":
    main()
