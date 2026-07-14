#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, values, queries):
    lines = [str(len(values)), " ".join(map(str, values)), str(len(queries))]
    lines.extend(f"{left + 1} {right + 1}" for left, right in queries)
    answers = [len(set(values[left:right + 1])) for left, right in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answers)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, 0, [5], [(0, 0)])
    rng = random.Random(76013)
    values = [rng.randrange(-8, 9) for _ in range(73)]
    queries = []
    for _ in range(150):
        left = rng.randrange(len(values))
        right = rng.randrange(left, len(values))
        queries.append((left, right))
    write_case(out_dir, 1, values, queries)


if __name__ == "__main__":
    main()
