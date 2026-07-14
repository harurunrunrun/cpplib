#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def largest_rectangle(heights):
    answer = 0
    for left in range(len(heights)):
        minimum = heights[left]
        for right in range(left, len(heights)):
            minimum = min(minimum, heights[right])
            answer = max(answer, minimum * (right - left + 1))
    return answer


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(19190)
    cases = [[0], [5], [2, 1, 4, 5, 1, 3, 3], [7, 7, 7, 7], list(range(1, 15))]
    for n in (2, 8, 25, 45):
        cases.append([rng.randrange(0, 31) for _ in range(n)])
    lines = [f"{len(values)} {' '.join(map(str, values))}" for values in cases]
    lines.append("0")
    answers = [str(largest_rectangle(values)) for values in cases]
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
