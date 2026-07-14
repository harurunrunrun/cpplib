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
    rng = random.Random(14140)
    size = 47
    lights = [0] * size
    commands = []
    answers = []
    for index in range(180):
        left = rng.randrange(size)
        right = rng.randrange(left, size)
        if index % 3:
            commands.append(f"0 {left + 1} {right + 1}")
            for position in range(left, right + 1):
                lights[position] ^= 1
        else:
            commands.append(f"1 {left + 1} {right + 1}")
            answers.append(str(sum(lights[left : right + 1])))
    lines = [f"{size} {len(commands)}", *commands]
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
