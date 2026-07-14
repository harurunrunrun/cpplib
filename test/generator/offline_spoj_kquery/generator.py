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
    rng = random.Random(15015)
    values = [0, -5, 8, 8, 21, -5, 13, 1, 34, 2]
    values.extend(rng.randint(-40, 40) for _ in range(35))
    queries = [(0, len(values) - 1, -100), (0, len(values) - 1, 100), (2, 4, 8)]
    for _ in range(100):
        left = rng.randrange(len(values))
        right = rng.randrange(left, len(values))
        threshold = rng.randint(-50, 50)
        queries.append((left, right, threshold))
    lines = [str(len(values)), " ".join(map(str, values)), str(len(queries))]
    lines.extend(f"{left + 1} {right + 1} {threshold}" for left, right, threshold in queries)
    answers = [str(sum(value > threshold for value in values[left : right + 1])) for left, right, threshold in queries]
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
