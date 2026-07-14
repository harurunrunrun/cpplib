#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def reconstruct(walked):
    available = list(range(1, len(walked) + 1))
    answer = [0] * len(walked)
    for index in range(len(walked) - 1, -1, -1):
        answer[index] = available.pop(len(available) - 1 - walked[index])
    return answer


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(6110)
    cases = [[0], [0, 0, 0, 0], [0, 1, 2, 3, 4]]
    for n in (2, 8, 25, 60):
        cases.append([rng.randrange(index + 1) for index in range(n)])
    lines = [str(len(cases))]
    answers = []
    for walked in cases:
        lines.append(str(len(walked)))
        lines.append(" ".join(map(str, walked)))
        answers.append(" ".join(map(str, reconstruct(walked))))
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
