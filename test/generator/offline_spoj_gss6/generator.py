#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def maximum_subarray(values, left, right):
    answer = -10**30
    for begin in range(left, right):
        total = 0
        for end in range(begin, right):
            total += values[end]
            answer = max(answer, total)
    return answer


def make_case(seed):
    rng = random.Random(seed)
    values = [7, -9, 4, -1, 3]
    initial = values[:]
    operations = []
    answers = []
    operations.extend(["Q 1 5", "R 2 -3", "Q 2 4"])
    values[1] = -3
    answers.extend([str(maximum_subarray(initial, 0, 5)), str(maximum_subarray(values, 1, 4))])
    for _ in range(70):
        kind = rng.choices("QIRD", weights=(5, 2, 2, 1))[0]
        if kind == "D" and len(values) == 1:
            kind = "I"
        if kind == "Q":
            left = rng.randrange(len(values))
            right = rng.randrange(left + 1, len(values) + 1)
            operations.append(f"Q {left + 1} {right}")
            answers.append(str(maximum_subarray(values, left, right)))
        elif kind == "I":
            position = rng.randrange(len(values) + 1)
            value = rng.randint(-15, 15)
            operations.append(f"I {position + 1} {value}")
            values.insert(position, value)
        elif kind == "R":
            position = rng.randrange(len(values))
            value = rng.randint(-15, 15)
            operations.append(f"R {position + 1} {value}")
            values[position] = value
        else:
            position = rng.randrange(len(values))
            operations.append(f"D {position + 1}")
            values.pop(position)
    lines = [str(len(initial)), " ".join(map(str, initial)), str(len(operations)), *operations]
    return "\n".join(lines) + "\n", "\n".join(answers) + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, seed in enumerate((6006, 6017)):
        data, expected = make_case(seed)
        (out_dir / f"case_{index:02d}.in").write_text(data, encoding="utf-8")
        (out_dir / f"case_{index:02d}.out").write_text(expected, encoding="utf-8")


if __name__ == "__main__":
    main()
