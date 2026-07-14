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

    rng = random.Random(2026071404)
    size = 47
    values = [rng.randrange(MOD) for _ in range(size)]
    initial = list(values)
    operations = []
    answers = []

    for left, right in [(0, size), (17, 17)]:
        operations.append((1, left, right))
        answers.append(str(sum(values[left:right]) % MOD))

    for _ in range(260):
        if rng.random() < 0.57:
            left = rng.randrange(size + 1)
            right = rng.randrange(left, size + 1)
            multiplier = rng.randrange(MOD)
            addend = rng.randrange(MOD)
            for index in range(left, right):
                values[index] = (multiplier * values[index] + addend) % MOD
            operations.append((0, left, right, multiplier, addend))
        else:
            left = rng.randrange(size + 1)
            right = rng.randrange(left, size + 1)
            operations.append((1, left, right))
            answers.append(str(sum(values[left:right]) % MOD))

    lines = [f"{size} {len(operations)}", " ".join(map(str, initial))]
    lines.extend(" ".join(map(str, operation)) for operation in operations)
    (out_dir / "random.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "random.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
