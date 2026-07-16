#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encoded(value: str) -> str:
    return value if value else "-"


def brute_length(first: str, second: str) -> int:
    previous = [0] * (len(second) + 1)
    answer = 0
    for left in first:
        current = [0] * (len(second) + 1)
        for index, right in enumerate(second, 1):
            if left == right:
                current[index] = previous[index - 1] + 1
                answer = max(answer, current[index])
        previous = current
    return answer


def write_case(
    out_dir: Path,
    index: int,
    cases: list[tuple[str, str, int]],
) -> None:
    lines = [str(len(cases))]
    lines.extend(
        f"{encoded(first)} {encoded(second)} {answer}"
        for first, second, answer in cases
    )
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join(lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{stem}.out").write_text("OK\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ("", "", 0),
        ("a", "", 0),
        ("banana", "ananas", 5),
        ("abcdef", "zabcy", 3),
        ("aaaa", "baaaab", 4),
    ]
    rng = random.Random(20260717)
    for _ in range(300):
        first = "".join(
            rng.choice("abcde") for _ in range(rng.randrange(35))
        )
        second = "".join(
            rng.choice("abcde") for _ in range(rng.randrange(35))
        )
        cases.append((first, second, brute_length(first, second)))
    write_case(out_dir, 0, cases)

    length = 300_000
    first = "x" * length + "a"
    second = "b" + "x" * length
    write_case(out_dir, 1, [(first, second, length)])


if __name__ == "__main__":
    main()
