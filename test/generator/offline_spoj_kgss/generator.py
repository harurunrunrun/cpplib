#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(16016)
    values = [5, -7, 5, 0, 12, -3, 12]
    initial = values[:]
    commands = []
    answers = []
    for index in range(120):
        if index % 4:
            left = rng.randrange(len(values) - 1)
            right = rng.randrange(left + 1, len(values))
            commands.append(f"Q {left + 1} {right + 1}")
            answers.append(str(sum(sorted(values[left : right + 1], reverse=True)[:2])))
        else:
            position = rng.randrange(len(values))
            value = rng.randint(-30, 30)
            commands.append(f"U {position + 1} {value}")
            values[position] = value
    lines = [str(len(initial)), " ".join(map(str, initial)), str(len(commands)), *commands]
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
