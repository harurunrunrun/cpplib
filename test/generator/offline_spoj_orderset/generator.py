#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import bisect
import random
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(6120)
    values = []
    commands = []
    answers = []
    fixed = [("K", 1), ("I", 5), ("I", 5), ("C", 6), ("D", 4), ("D", 5), ("K", 1)]
    for operation, argument in fixed:
        commands.append(f"{operation} {argument}")
        if operation == "I":
            position = bisect.bisect_left(values, argument)
            if position == len(values) or values[position] != argument:
                values.insert(position, argument)
        elif operation == "D":
            position = bisect.bisect_left(values, argument)
            if position < len(values) and values[position] == argument:
                values.pop(position)
        elif operation == "C":
            answers.append(str(bisect.bisect_left(values, argument)))
        else:
            answers.append(str(values[argument - 1]) if 1 <= argument <= len(values) else "invalid")
    for _ in range(220):
        operation = rng.choices("IDCK", weights=(4, 3, 3, 3))[0]
        argument = rng.randint(-80, 80) if operation != "K" else rng.randint(0, len(values) + 3)
        commands.append(f"{operation} {argument}")
        if operation == "I":
            position = bisect.bisect_left(values, argument)
            if position == len(values) or values[position] != argument:
                values.insert(position, argument)
        elif operation == "D":
            position = bisect.bisect_left(values, argument)
            if position < len(values) and values[position] == argument:
                values.pop(position)
        elif operation == "C":
            answers.append(str(bisect.bisect_left(values, argument)))
        else:
            answers.append(str(values[argument - 1]) if 1 <= argument <= len(values) else "invalid")
    lines = [str(len(commands)), *commands]
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
