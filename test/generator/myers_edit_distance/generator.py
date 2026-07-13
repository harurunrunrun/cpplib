#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Case = tuple[bytes, bytes]


def naive_edit_distance(first: bytes, second: bytes) -> int:
    if len(first) > len(second):
        first, second = second, first
    distance = list(range(len(first) + 1))
    for second_index, second_value in enumerate(second):
        diagonal = distance[0]
        distance[0] = second_index + 1
        for first_index, first_value in enumerate(first):
            old = distance[first_index + 1]
            distance[first_index + 1] = min(
                old + 1,
                distance[first_index] + 1,
                diagonal + (first_value != second_value),
            )
            diagonal = old
    return distance[-1]


def write_case(
    out_dir: Path,
    name: str,
    cases: list[Case],
    expected: list[int] | None = None,
) -> None:
    if expected is None:
        expected = [
            naive_edit_distance(first, second)
            for first, second in cases
        ]

    input_lines = [str(len(cases))]
    for first, second in cases:
        input_lines.append(f"{len(first)} {len(second)}")
        input_lines.append(" ".join(map(str, first)))
        input_lines.append(" ".join(map(str, second)))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, expected)) + "\n",
        encoding="utf-8",
    )


def boundary_cases() -> list[Case]:
    cases: list[Case] = [
        (b"", b""),
        (b"", b"abc"),
        (b"a", b""),
        (b"kitten", b"sitting"),
        (bytes(range(256)), bytes(reversed(range(256)))),
    ]
    for length in [1, 2, 62, 63, 64, 65, 66, 127, 128, 129]:
        pattern = bytes(97 + index % 5 for index in range(length))
        changed = bytearray(pattern)
        changed[length // 2] = ord("z")
        inserted = (
            pattern[: length // 2]
            + b"#"
            + pattern[length // 2 :]
        )
        erased = (
            pattern[: length // 2]
            + pattern[length // 2 + 1 :]
        )
        cases.extend(
            [
                (pattern, pattern),
                (pattern, bytes(changed)),
                (pattern, inserted),
                (pattern, erased),
            ]
        )
    return cases


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "boundaries", boundary_cases())

    for seed in range(6):
        rng = random.Random(20260803 + seed)
        cases: list[Case] = []
        for case_index in range(32):
            first_size = rng.randrange(221)
            second_size = rng.randrange(221)
            alphabet_size = (
                256 if case_index % 7 == 0 else rng.randrange(1, 9)
            )
            first = bytes(
                rng.randrange(alphabet_size)
                for _ in range(first_size)
            )
            second = bytes(
                rng.randrange(alphabet_size)
                for _ in range(second_size)
            )
            cases.append((first, second))
        write_case(out_dir, f"random_{seed:02d}", cases)

    first = b"a" * 30000
    second = b"a" * 45000
    disjoint_first = b"a" * 16000
    disjoint_second = b"b" * 22000
    repeated = bytes(97 + index % 4 for index in range(20000))
    write_case(
        out_dir,
        "large",
        [
            (first, second),
            (disjoint_first, disjoint_second),
            (repeated, repeated + repeated),
        ],
        [15000, 22000, 20000],
    )


if __name__ == "__main__":
    main()
