#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def make_case(rng, size, command_count):
    matrix = [[0] * size for _ in range(size)]
    commands = []
    answers = []
    for index in range(command_count):
        if index % 3:
            x = rng.randrange(size)
            y = rng.randrange(size)
            value = rng.randint(-1000, 1000)
            commands.append(f"SET {x} {y} {value}")
            matrix[y][x] = value
        else:
            x1 = rng.randrange(size)
            x2 = rng.randrange(x1, size)
            y1 = rng.randrange(size)
            y2 = rng.randrange(y1, size)
            commands.append(f"SUM {x1} {y1} {x2} {y2}")
            answers.append(str(sum(matrix[y][x] for y in range(y1, y2 + 1) for x in range(x1, x2 + 1))))
    return commands, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(13130)
    cases = [make_case(rng, 1, 18), make_case(rng, 7, 90), make_case(rng, 16, 130)]
    lines = [str(len(cases))]
    expected = []
    for size, (commands, answers) in zip((1, 7, 16), cases):
        lines.append(str(size))
        lines.extend(commands)
        lines.append("END")
        expected.extend(answers)
        expected.append("")
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(expected) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
