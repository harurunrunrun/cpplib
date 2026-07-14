#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import collections
import random
from pathlib import Path


def representation(patterns: list[str]) -> tuple[list[int], list[int], list[int]]:
    transitions: list[list[int]] = [[0, 0, 0]]
    parent = [0]
    terminal: list[int] = []
    for pattern in patterns:
        state = 0
        for character in pattern:
            index = ord(character) - ord("a")
            if transitions[state][index] == 0:
                transitions[state][index] = len(transitions)
                transitions.append([0, 0, 0])
                parent.append(state)
            state = transitions[state][index]
        terminal.append(state)

    failure = [0] * len(transitions)
    queue: collections.deque[int] = collections.deque()
    for character in range(3):
        state = transitions[0][character]
        if state != 0:
            queue.append(state)
    while queue:
        state = queue.popleft()
        for character in range(3):
            to = transitions[state][character]
            if to == 0:
                transitions[state][character] = transitions[failure[state]][character]
            else:
                failure[to] = transitions[failure[state]][character]
                queue.append(to)
    return parent, failure, terminal


def write_case(out_dir: Path, index: int, patterns: list[str]) -> None:
    parent, failure, terminal = representation(patterns)
    input_lines = [str(len(patterns)), *patterns]
    output_lines = [str(len(parent))]
    output_lines.extend(
        f"{parent[node]} {failure[node]}" for node in range(1, len(parent))
    )
    output_lines.append(" ".join(map(str, terminal)))
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    arguments = parser.parse_args()
    out_dir = Path(arguments.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        ["a"],
        ["ab", "b", "ab"],
        ["a", "aa", "aaa", "baaa"],
        ["abc", "bca", "cab", "c"],
    ]
    for index, patterns in enumerate(fixed):
        write_case(out_dir, index, patterns)

    rng = random.Random(20260714)
    for index in range(len(fixed), len(fixed) + 16):
        count = rng.randrange(1, 31)
        patterns = [
            "".join(rng.choice("abc") for _ in range(rng.randrange(1, 13)))
            for _ in range(count)
        ]
        write_case(out_dir, index, patterns)


if __name__ == "__main__":
    main()
