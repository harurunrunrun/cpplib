#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


DEFAULT_MODULUS = 1_000_000_000
STATIC_MODULUS = 998_244_353


def recurrence(initial, coefficient, index, modulus):
    values = [value % modulus for value in initial]
    if index <= len(values):
        return values[index - 1]
    while len(values) < index:
        value = sum(
            coefficient[offset] * values[-1 - offset]
            for offset in range(len(coefficient))
        )
        values.append(value % modulus)
    return values[index - 1]


class Case:
    def __init__(self):
        self.queries = []
        self.answers = []

    def add(self, mode, initial, coefficient, index, modulus=None, answer=None):
        if mode == "DEFAULT":
            actual_modulus = DEFAULT_MODULUS
        elif mode == "STATIC998":
            actual_modulus = STATIC_MODULUS
        else:
            assert mode == "RUNTIME" and modulus is not None
            actual_modulus = modulus
        header = f"{mode} {len(initial)} {index}"
        if mode == "RUNTIME":
            header += f" {actual_modulus}"
        self.queries.extend([
            header,
            " ".join(map(str, initial)),
            " ".join(map(str, coefficient)),
        ])
        if answer is None:
            answer = recurrence(
                initial, coefficient, index, actual_modulus
            )
        self.answers.append(str(answer))

    def write(self, out_dir, stem):
        (out_dir / f"{stem}.in").write_text(
            str(len(self.answers)) + "\n"
            + "\n".join(self.queries) + "\n",
            encoding="utf-8",
        )
        (out_dir / f"{stem}.out").write_text(
            "\n".join(self.answers) + "\n",
            encoding="utf-8",
        )


def boundary_case():
    case = Case()
    case.add("DEFAULT", [7], [13], 1)
    case.add("DEFAULT", [0, 1], [1, 1], 93)
    case.add("STATIC998", [5, 8, 13], [2, 3, 5], 3)
    case.add("STATIC998", [5, 8, 13], [2, 3, 5], 140)
    case.add("RUNTIME", [9], [17], 80, 1)
    case.add("RUNTIME", [9], [17], 80, 97)
    case.add(
        "RUNTIME", [(1 << 64) - 1, (1 << 63) + 17], [123, (1 << 64) - 3],
        90, (1 << 64) - 59,
    )
    return case


def random_case():
    rng = random.Random(2026071701)
    case = Case()
    modes = ("DEFAULT", "STATIC998", "RUNTIME")
    runtime_moduli = (2, 97, 1_000_000_007, (1 << 64) - 59)
    for query in range(120):
        degree = rng.randrange(1, 13)
        initial = [rng.getrandbits(64) for _ in range(degree)]
        coefficient = [rng.getrandbits(64) for _ in range(degree)]
        index = rng.randrange(1, 180)
        mode = modes[query % len(modes)]
        modulus = runtime_moduli[query % len(runtime_moduli)]
        case.add(mode, initial, coefficient, index, modulus)
    return case


def performance_case():
    degree = 8192
    index = 1_000_000_000_000_000_123
    initial = [
        (position * position + 17 * position + 3) % DEFAULT_MODULUS
        for position in range(degree)
    ]
    coefficient = [0] * degree
    coefficient[-1] = 1
    case = Case()
    case.add(
        "DEFAULT", initial, coefficient, index,
        answer=initial[(index - 1) % degree],
    )
    return case


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    for index, case in enumerate((
        boundary_case(), random_case(), performance_case()
    )):
        case.write(args.out_dir, f"case_{index:02d}")


if __name__ == "__main__":
    main()
