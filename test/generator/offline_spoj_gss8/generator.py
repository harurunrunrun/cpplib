#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


MODULUS = 1 << 32


def cast(value):
    return value % MODULUS


def moment(values, left, right, power):
    return sum(
        value * pow(position, power, MODULUS)
        for position, value in enumerate(values[left : right + 1], 1)
    ) % MODULUS


def make_case(seed):
    rng = random.Random(seed)
    values = [0, 1, MODULUS - 1, 123456789]
    initial = values[:]
    operations = []
    answers = []
    for index in range(90):
        kind = rng.choices("QIRD", weights=(6, 2, 2, 1))[0]
        if kind == "D" and len(values) == 1:
            kind = "I"
        if kind == "Q":
            left = rng.randrange(len(values))
            right = rng.randrange(left, len(values))
            power = rng.randrange(11)
            operations.append(f"Q {left} {right} {power}")
            answers.append(str(moment(values, left, right, power)))
        elif kind == "I":
            position = rng.randrange(len(values) + 1)
            raw = rng.randrange(0, 2 * MODULUS)
            operations.append(f"I {position} {raw}")
            values.insert(position, cast(raw))
        elif kind == "R":
            position = rng.randrange(len(values))
            raw = rng.randrange(0, 2 * MODULUS)
            operations.append(f"R {position} {raw}")
            values[position] = cast(raw)
        else:
            position = rng.randrange(len(values))
            operations.append(f"D {position}")
            values.pop(position)
    lines = [str(len(initial)), " ".join(map(str, initial)), str(len(operations)), *operations]
    return "\n".join(lines) + "\n", "\n".join(answers) + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, seed in enumerate((8008, 8021)):
        data, expected = make_case(seed)
        (out_dir / f"case_{index:02d}.in").write_text(data, encoding="utf-8")
        (out_dir / f"case_{index:02d}.out").write_text(expected, encoding="utf-8")


if __name__ == "__main__":
    main()
