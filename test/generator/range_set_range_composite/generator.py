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

    rng = random.Random(2026071406)
    size = 41
    functions = [
        (rng.randrange(MOD), rng.randrange(MOD)) for _ in range(size)
    ]
    initial = list(functions)
    operations = []
    answers = []

    for _ in range(250):
        if rng.random() < 0.52:
            left = rng.randrange(size + 1)
            right = rng.randrange(left, size + 1)
            multiplier = rng.randrange(MOD)
            addend = rng.randrange(MOD)
            for index in range(left, right):
                functions[index] = (multiplier, addend)
            operations.append((0, left, right, multiplier, addend))
        else:
            left = rng.randrange(size + 1)
            right = rng.randrange(left, size + 1)
            value = rng.randrange(MOD)
            result = value
            for multiplier, addend in functions[left:right]:
                result = (multiplier * result + addend) % MOD
            operations.append((1, left, right, value))
            answers.append(str(result))

    operations.append((1, 7, 7, MOD - 1))
    answers.append(str(MOD - 1))

    lines = [f"{size} {len(operations)}"]
    lines.extend(f"{a} {b}" for a, b in initial)
    lines.extend(" ".join(map(str, operation)) for operation in operations)
    (out_dir / "random.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "random.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
