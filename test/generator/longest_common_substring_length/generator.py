#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def longest_common_substring_length(first: bytes, second: bytes) -> int:
    previous = [0] * (len(second) + 1)
    answer = 0
    for left in first:
        current = [0] * (len(second) + 1)
        for index, right in enumerate(second):
            if left == right:
                current[index + 1] = previous[index] + 1
                answer = max(answer, current[index + 1])
        previous = current
    return answer


def encode(value: bytes) -> str:
    return value.hex() if value else "-"


def write_case(out_dir: Path, name: str, cases: list[tuple[bytes, bytes, int | None]]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for first, second, expected in cases:
        input_lines.append(f"{encode(first)} {encode(second)}")
        if expected is None:
            expected = longest_common_substring_length(first, second)
        output_lines.append(str(expected))
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    all_bytes = bytes(range(256))
    write_case(
        out_dir,
        "hand",
        [
            (b"", b"", 0),
            (b"", b"abc", 0),
            (b"abc", b"", 0),
            (b"a", b"a", 1),
            (b"abc", b"zabcy", 3),
            (b"banana", b"ananas", 5),
            (b"\x00\xff\x00\x80", b"\x01\x00\xff\x00\x02", 3),
            (all_bytes, all_bytes[::-1], 1),
            (all_bytes, b"prefix" + all_bytes[37:183] + b"suffix", 146),
        ],
    )

    for seed in range(5):
        rng = random.Random(20260714 + seed)
        cases: list[tuple[bytes, bytes, int | None]] = []
        alphabet_size = [1, 4, 8, 17, 256][seed]
        alphabet = list(range(alphabet_size))
        for _ in range(45):
            first = bytes(rng.choice(alphabet) for _ in range(rng.randrange(55)))
            second = bytes(rng.choice(alphabet) for _ in range(rng.randrange(55)))
            cases.append((first, second, None))
        write_case(out_dir, f"random_{seed:02d}", cases)

    repeated = b"\x00\xff" * 100_000
    write_case(
        out_dir,
        "large",
        [
            (repeated, b"\x01" * 1_000 + repeated, len(repeated)),
            (b"^v-~" * 50_000, b"x" * 1_000 + b"^v-~" * 50_000, 200_000),
        ],
    )


if __name__ == "__main__":
    main()
