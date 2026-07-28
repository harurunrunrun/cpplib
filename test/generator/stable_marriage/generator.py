#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def solve(
    proposer_preferences: list[list[int]],
    acceptor_preferences: list[list[int]],
) -> list[int]:
    proposer_count = len(proposer_preferences)
    rank = [
        {proposer: index for index, proposer in enumerate(row)}
        for row in acceptor_preferences
    ]
    proposer_match = [-1] * proposer_count
    acceptor_match = [-1] * len(acceptor_preferences)
    next_index = [0] * proposer_count
    free = collections.deque(range(proposer_count))
    while free:
        proposer = free.popleft()
        while next_index[proposer] < len(proposer_preferences[proposer]):
            acceptor = proposer_preferences[proposer][next_index[proposer]]
            next_index[proposer] += 1
            if proposer not in rank[acceptor]:
                continue
            previous = acceptor_match[acceptor]
            if previous == -1 or rank[acceptor][proposer] < rank[acceptor][previous]:
                proposer_match[proposer] = acceptor
                acceptor_match[acceptor] = proposer
                if previous != -1:
                    proposer_match[previous] = -1
                    free.append(previous)
                break
    return proposer_match


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    cases: list[tuple[list[list[int]], list[list[int]]]] = [
        ([], []),
        ([[0], [0]], [[1, 0]]),
        ([[0, 1], [1, 0]], [[1, 0], [0, 1]]),
    ]
    for _ in range(180):
        proposer_count = rng.randint(0, 12)
        acceptor_count = rng.randint(0, 12)
        proposer_preferences = []
        for _ in range(proposer_count):
            row = list(range(acceptor_count))
            rng.shuffle(row)
            proposer_preferences.append(row[:rng.randint(0, acceptor_count)])
        acceptor_preferences = []
        for _ in range(acceptor_count):
            row = list(range(proposer_count))
            rng.shuffle(row)
            acceptor_preferences.append(row[:rng.randint(0, proposer_count)])
        cases.append((proposer_preferences, acceptor_preferences))
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for proposer_preferences, acceptor_preferences in cases:
        input_lines.append(
            f"{len(proposer_preferences)} {len(acceptor_preferences)}"
        )
        for row in [*proposer_preferences, *acceptor_preferences]:
            input_lines.append(" ".join(map(str, [len(row), *row])))
        output_lines.append(" ".join(map(
            str, solve(proposer_preferences, acceptor_preferences)
        )))
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
