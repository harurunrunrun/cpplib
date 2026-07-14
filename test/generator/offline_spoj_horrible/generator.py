#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def make_case(rng, size, command_count):
    values = [0] * size
    commands = []
    answers = []
    for index in range(command_count):
        left = rng.randrange(size)
        right = rng.randrange(left, size)
        if index % 3:
            commands.append(f"0 {left + 1} {right + 1} {rng.randint(-100, 100)}")
            delta = int(commands[-1].split()[-1])
            for position in range(left, right + 1):
                values[position] += delta
        else:
            commands.append(f"1 {left + 1} {right + 1}")
            answers.append(str(sum(values[left : right + 1])))
    return commands, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(18181)
    cases = []
    for size, count in ((1, 30), (9, 90), (31, 160)):
        commands, answers = make_case(rng, size, count)
        cases.append((size, commands, answers))
    lines = [str(len(cases))]
    expected = []
    for size, commands, answers in cases:
        lines.append(f"{size} {len(commands)}")
        lines.extend(commands)
        expected.extend(answers)
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(expected) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
