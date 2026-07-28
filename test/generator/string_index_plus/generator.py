#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def bwt(text: str) -> tuple[int, str]:
    rotations = sorted((text[index:] + text[:index], index)
                       for index in range(len(text)))
    transformed = "".join(rotation[-1] for rotation, _ in rotations)
    primary = next(
        row for row, (_, start) in enumerate(rotations) if start == 0
    )
    return primary, transformed


def positions(text: str, pattern: str) -> list[int]:
    return [
        index
        for index in range(len(text) - len(pattern) + 1)
        if text.startswith(pattern, index)
    ]


def lce(text: str, first: int, second: int) -> int:
    result = 0
    while (
        first + result < len(text)
        and second + result < len(text)
        and text[first + result] == text[second + result]
    ):
        result += 1
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    alphabet = "abcde"
    cases: list[tuple[int, object, list[str]]] = []
    for _ in range(100):
        text = "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 90)))
        primary, transformed = bwt(text)
        cases.append((0, text, [f"{primary} {transformed}"]))
    for _ in range(80):
        text = "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 120)))
        patterns = [
            "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 15)))
            for _ in range(rng.randint(1, 25))
        ]
        expected = []
        for pattern in patterns:
            found = positions(text, pattern)
            expected.append(" ".join(map(str, [len(found), *found])))
        cases.append((1, (text, patterns), expected))
    for _ in range(120):
        text = "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 160)))
        pattern = "".join(
            rng.choice(alphabet) for _ in range(rng.randint(1, 130))
        )
        found = positions(text, pattern)
        cases.append((2, (text, pattern), [
            " ".join(map(str, [len(found), *found]))
        ]))
    for _ in range(80):
        text = "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 150)))
        queries = [
            (rng.randint(0, len(text)), rng.randint(0, len(text)))
            for _ in range(rng.randint(1, 35))
        ]
        cases.append((3, (text, queries), [
            str(lce(text, first, second)) for first, second in queries
        ]))
    for _ in range(80):
        text = "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 100)))
        current = list(text)
        operations = []
        expected = []
        for _ in range(rng.randint(1, 70)):
            if rng.random() < 0.4:
                position = rng.randrange(len(current))
                symbol = rng.choice(alphabet)
                current[position] = symbol
                operations.append((0, position, symbol))
            else:
                first = rng.randint(0, len(current))
                second = rng.randint(0, len(current))
                operations.append((1, first, second))
                expected.append(str(lce("".join(current), first, second)))
        cases.append((4, (text, operations), expected))
    input_lines = [str(len(cases))]
    output_lines = []
    for kind, data, expected in cases:
        if kind == 0:
            input_lines.append(f"0 {data}")
        elif kind == 1:
            text, patterns = data
            input_lines.append(f"1 {text} {len(patterns)}")
            input_lines.extend(patterns)
        elif kind == 2:
            text, pattern = data
            input_lines.append(f"2 {text} {pattern}")
        elif kind == 3:
            text, queries = data
            input_lines.append(f"3 {text} {len(queries)}")
            input_lines.extend(f"{first} {second}" for first, second in queries)
        else:
            text, operations = data
            input_lines.append(f"4 {text} {len(operations)}")
            input_lines.extend(" ".join(map(str, operation))
                               for operation in operations)
        output_lines.extend(expected)
    (args.out_dir / "cases.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (args.out_dir / "cases.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
