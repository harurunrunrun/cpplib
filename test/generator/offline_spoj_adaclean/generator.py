#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from pathlib import Path


MAX_LENGTH = 100_000
MAX_TOTAL_LENGTH = 300_000


def solve_brute(text: str, length: int) -> int:
    return len({text[start:start + length] for start in range(len(text) - length + 1)})


def write_suite(
    out_dir: Path,
    name: str,
    cases: list[tuple[str, int, int | None]],
    *,
    final_newline: bool = True,
) -> None:
    assert cases
    assert sum(len(text) for text, _, _ in cases) <= MAX_TOTAL_LENGTH
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for text, length, answer in cases:
        assert 1 <= length <= len(text) <= MAX_LENGTH
        input_lines.extend([f"{len(text)} {length}", text])
        if answer is None:
            answer = solve_brute(text, length)
        output_lines.append(str(answer))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + ("\n" if final_newline else ""),
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_suite(
        out_dir,
        "official",
        [
            ("aaa", 2, 1),
            ("abcba", 1, 3),
            ("abac", 2, 3),
            ("abbaaaabba", 2, 4),
            ("dogodog", 3, 4),
        ],
    )

    rng = random.Random(300000100)
    random_cases: list[tuple[str, int, int | None]] = []
    for _ in range(200):
        text = "".join(rng.choice("abcdef") for _ in range(rng.randint(1, 100)))
        random_cases.append((text, rng.randint(1, len(text)), None))
    write_suite(out_dir, "random_bruteforce", random_cases)

    cycle = (string.ascii_lowercase * (MAX_LENGTH // 26 + 1))[:MAX_LENGTH]
    write_suite(
        out_dir,
        "maximum_total_length",
        [
            ("a" * MAX_LENGTH, 50_000, 1),
            (cycle, 50_000, 26),
            (cycle, 1, 26),
        ],
        final_newline=False,
    )

    write_suite(
        out_dir,
        "whole_string",
        [(cycle, MAX_LENGTH, 1)],
    )


if __name__ == "__main__":
    main()
