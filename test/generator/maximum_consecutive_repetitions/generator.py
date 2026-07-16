#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
from pathlib import Path


def solve_naive(text: bytes) -> int:
    """Independent cubic oracle which compares adjacent blocks byte by byte."""
    if not text:
        return 0
    answer = 1
    for left in range(len(text)):
        for period in range(1, len(text) - left + 1):
            count = 1
            while left + (count + 1) * period <= len(text):
                equal = True
                for offset in range(period):
                    if (
                        text[left + offset]
                        != text[left + count * period + offset]
                    ):
                        equal = False
                        break
                if not equal:
                    break
                count += 1
            answer = max(answer, count)
    return answer


def write_case(
    out_dir: Path,
    name: str,
    cases: list[tuple[bytes, int]],
) -> None:
    encoded = [text.hex() if text else "-" for text, _ in cases]
    (out_dir / f"{name}.in").write_text(
        "\n".join([str(len(cases)), *encoded]) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(str(answer) for _, answer in cases) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        b"a",
        b"aaaaa",
        b"abababab",
        b"xababab",
        b"abcabca",
        b"zabcabcabcx",
        b"aabaabaa",
        b"banana",
        b"mississippi",
        b"abcdef",
        bytes([0, 255, 127, 128]) * 3,
    ]
    fixed_cases = [(text, solve_naive(text)) for text in fixed]
    fixed_cases.extend([
        (bytes(range(256)), 1),
        (bytes(range(256)) * 2, 2),
    ])
    write_case(out_dir, "fixed_and_all_bytes", fixed_cases)

    exhaustive_binary: list[tuple[bytes, int]] = []
    for length in range(1, 13):
        for values in itertools.product((0, 255), repeat=length):
            text = bytes(values)
            exhaustive_binary.append((text, solve_naive(text)))
    write_case(out_dir, "exhaustive_binary", exhaustive_binary)

    exhaustive_ternary: list[tuple[bytes, int]] = []
    for length in range(1, 10):
        for values in itertools.product((0, 127, 255), repeat=length):
            text = bytes(values)
            exhaustive_ternary.append((text, solve_naive(text)))
    write_case(out_dir, "exhaustive_ternary", exhaustive_ternary)

    for seed in range(6):
        rng = random.Random(82000 + seed)
        texts = [
            bytes(rng.randrange(256) for _ in range(rng.randint(1, 45)))
            for _ in range(80)
        ]
        for _ in range(40):
            period = rng.randint(1, 12)
            repetitions = rng.randint(2, 8)
            root = bytes(rng.randrange(8) for _ in range(period))
            prefix = bytes(rng.randrange(8) for _ in range(rng.randint(0, 5)))
            suffix = bytes(rng.randrange(8) for _ in range(rng.randint(0, 5)))
            texts.append(prefix + root * repetitions + suffix)
        write_case(
            out_dir,
            f"random_and_period_boundaries_{seed:02d}",
            [(text, solve_naive(text)) for text in texts],
        )

    size = 400_000
    increasing_root = bytes(range(251))
    repeated_count = size // len(increasing_root)
    write_case(
        out_dir,
        "large_linear",
        [
            (b"a" * size, size),
            (b"\0" + b"\xff" * (size - 1), size - 1),
            (increasing_root * repeated_count, repeated_count),
            (b"x" + b"abc" * 120_000 + b"y", 120_000),
        ],
    )


if __name__ == "__main__":
    main()
