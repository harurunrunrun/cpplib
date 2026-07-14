#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path

MOD = 998244353


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071405)
    size = 1_000_000_000
    coordinates = {
        0,
        1,
        2,
        123_456,
        500_000_000,
        size - 2,
        size - 1,
        size,
    }
    while len(coordinates) < 58:
        coordinates.add(rng.randrange(size + 1))
    coordinates = sorted(coordinates)
    values = [0] * (len(coordinates) - 1)
    operations = []
    answers = []

    for _ in range(240):
        left_index = rng.randrange(len(coordinates))
        right_index = rng.randrange(left_index, len(coordinates))
        left = coordinates[left_index]
        right = coordinates[right_index]
        if rng.random() < 0.58:
            multiplier = rng.randrange(MOD)
            addend = rng.randrange(MOD)
            for index in range(left_index, right_index):
                values[index] = (multiplier * values[index] + addend) % MOD
            operations.append((0, left, right, multiplier, addend))
        else:
            result = 0
            for index in range(left_index, right_index):
                length = coordinates[index + 1] - coordinates[index]
                result = (result + values[index] * length) % MOD
            operations.append((1, left, right))
            answers.append(str(result))

    operations.append((1, 0, size))
    result = sum(
        value * (coordinates[index + 1] - coordinates[index])
        for index, value in enumerate(values)
    ) % MOD
    answers.append(str(result))

    lines = [f"{size} {len(operations)}"]
    lines.extend(" ".join(map(str, operation)) for operation in operations)
    (out_dir / "random.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "random.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
