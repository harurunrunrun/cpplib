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
    rng = random.Random(11110)
    values = [rng.randint(-100, 100) for _ in range(70)]
    values[:8] = [5, 5, -3, 8, -3, 0, 100, -100]
    queries = [(0, len(values) - 1, 0), (0, len(values) - 1, len(values) - 1), (2, 6, 2)]
    for _ in range(130):
        left = rng.randrange(len(values))
        right = rng.randrange(left, len(values))
        order = rng.randrange(right - left + 1)
        queries.append((left, right, order))
    lines = [f"{len(values)} {len(queries)}", " ".join(map(str, values))]
    lines.extend(f"{left + 1} {right + 1} {order + 1}" for left, right, order in queries)
    answers = [str(sorted(values[left : right + 1])[order]) for left, right, order in queries]
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
