#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

PARAMETERS = {
    "known32": (
        32,
        (
            0xCD9E8D57,
            0x9E3779B9,
            0xD2511F53,
            0xBB67AE85,
        ),
    ),
    "known64": (
        64,
        (
            0xCA5A826395121157,
            0x9E3779B97F4A7C15,
            0xD2E7470EE14C6C93,
            0xBB67AE8584CAA73B,
        ),
    ),
}
OBSERVATION_COUNT = 11


def counter_words(value: int, word_size: int) -> list[int]:
    mask = (1 << word_size) - 1
    return [
        (value >> (word_size * index)) & mask
        for index in reversed(range(4))
    ]


def generate_block(
    counter: list[int],
    key: list[int],
    word_size: int,
    constants: tuple[int, ...],
) -> list[int]:
    mask = (1 << word_size) - 1
    multipliers = constants[0::2]
    round_constants = constants[1::2]
    current = list(reversed(counter))
    for round_index in range(10):
        shuffled = [current[2], current[1], current[0], current[3]]
        following = [0] * 4
        for index in range(2):
            product = shuffled[2 * index] * multipliers[index]
            high = (product >> word_size) & mask
            low = product & mask
            round_key = (
                key[index] + round_index * round_constants[index]
            ) & mask
            following[2 * index] = (
                high ^ round_key ^ shuffled[2 * index + 1]
            ) & mask
            following[2 * index + 1] = low
        current = following
    return current


def stream(
    mode: str,
    seed: int,
    start: int,
    count: int,
) -> list[int]:
    word_size, constants = PARAMETERS[mode]
    mask = (1 << word_size) - 1
    key = [seed & mask, 0]
    block_number, index = divmod(start, 4)
    answer: list[int] = []
    while len(answer) < count:
        block = generate_block(
            counter_words(block_number, word_size),
            key,
            word_size,
            constants,
        )
        take = min(count - len(answer), 4 - index)
        answer.extend(block[index:index + take])
        block_number += 1
        index = 0
    return answer


Case = tuple[str, int, int, int]


def expected(case: Case) -> str:
    mode, seed, discard_count, prediction_count = case
    predictions = stream(
        mode,
        seed,
        discard_count + OBSERVATION_COUNT,
        prediction_count,
    )
    return "1" + "".join(f" {value}" for value in predictions)


def write_batch(out_dir: Path, name: str, cases: list[Case]) -> None:
    (out_dir / f"{name}.in").write_text(
        "\n".join(
            [
                "batch",
                str(len(cases)),
                *(
                    f"{mode} {seed} {discard_count} {prediction_count}"
                    for mode, seed, discard_count, prediction_count in cases
                ),
            ]
        )
        + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(expected(case) for case in cases) + "\n",
        encoding="utf-8",
    )


def write_one_round_batch(
    out_dir: Path,
    cases: list[tuple[int, int, int]],
) -> None:
    (out_dir / "0006_one_round.in").write_text(
        "\n".join(
            [
                "one_round_batch",
                str(len(cases)),
                *(f"{seed} {first} {second}" for seed, first, second in cases),
            ]
        )
        + "\n",
        encoding="utf-8",
    )
    (out_dir / "0006_one_round.out").write_text(
        "\n".join(f"1 1 {seed & 3}" for seed, _, _ in cases) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    modes = ("edges", "maximum_aliases", "errors")
    for index, mode in enumerate(modes):
        stem = f"{index:04d}_legacy"
        (args.out_dir / f"{stem}.in").write_text(
            mode + "\n",
            encoding="utf-8",
        )
        (args.out_dir / f"{stem}.out").write_text(
            "OK\n",
            encoding="utf-8",
        )

    maximum = (1 << 64) - 1
    boundary_cases: list[Case] = []
    for mode in PARAMETERS:
        seed_maximum = (1 << PARAMETERS[mode][0]) - 1
        for seed in (0, 1, seed_maximum):
            for discard_count in (0, 1, 3, 4, 5, 1000, maximum):
                boundary_cases.append(
                    (mode, seed, discard_count, 64)
                )
    write_batch(
        args.out_dir,
        "0003_boundaries",
        boundary_cases,
    )

    rng = random.Random(2026071610)
    random_cases: list[Case] = []
    for index in range(1024):
        mode = "known32" if index % 2 == 0 else "known64"
        word_size = PARAMETERS[mode][0]
        random_cases.append(
            (
                mode,
                rng.getrandbits(word_size),
                rng.getrandbits(64),
                rng.randrange(1, 97),
            )
        )
    write_batch(args.out_dir, "0004_random", random_cases)

    large_cases: list[Case] = []
    for index in range(4096):
        mode = "known32" if index % 2 == 0 else "known64"
        word_size = PARAMETERS[mode][0]
        large_cases.append(
            (
                mode,
                rng.getrandbits(word_size),
                rng.getrandbits(64),
                8,
            )
        )
    write_batch(args.out_dir, "0005_large", large_cases)

    one_round_cases = [
        (seed, first, second)
        for seed in (0, 1, 3, 4, 255)
        for first in range(4)
        for second in range(4)
    ]
    for index in range(4096):
        one_round_cases.append(
            (index & 255, rng.randrange(4), rng.randrange(4))
        )
    write_one_round_batch(args.out_dir, one_round_cases)


if __name__ == "__main__":
    main()
