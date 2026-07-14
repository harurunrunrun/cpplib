#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path

MOD = 998244353


def evaluate(functions, left, right, value):
    result = value
    for index in sorted(functions):
        if left <= index < right:
            multiplier, addend = functions[index]
            result = (multiplier * result + addend) % MOD
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071402)
    size = 1_000_000_000
    functions = {}
    operations = []
    answers = []

    for index, multiplier, addend in [
        (0, 2, 3),
        (size - 1, 5, 7),
        (123_456_789, MOD - 1, MOD - 2),
    ]:
        functions[index] = (multiplier, addend)
        operations.append((0, index, multiplier, addend))
    for left, right, value in [(0, size, 11), (7, 7, 13)]:
        operations.append((1, left, right, value))
        answers.append(str(evaluate(functions, left, right, value)))

    for _ in range(240):
        if rng.random() < 0.52:
            index = rng.randrange(size)
            multiplier = rng.randrange(MOD)
            addend = rng.randrange(MOD)
            functions[index] = (multiplier, addend)
            operations.append((0, index, multiplier, addend))
        else:
            left = rng.randrange(size + 1)
            right = rng.randrange(left, size + 1)
            value = rng.randrange(MOD)
            operations.append((1, left, right, value))
            answers.append(str(evaluate(functions, left, right, value)))

    lines = [f"{size} {len(operations)}"]
    lines.extend(" ".join(map(str, operation)) for operation in operations)
    (out_dir / "random.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "random.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
