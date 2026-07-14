#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, values, queries):
    answer = [min(values[min(left, right):max(left, right) + 1])
              for left, right in queries]
    lines = [str(len(values)), " ".join(map(str, values)), str(len(queries))]
    lines += [f"{left} {right}" for left, right in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answer)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([7], [(0, 0)]), ([4, -2, 8, -9], [(0, 3), (2, 1), (2, 2)])]
    rng = random.Random(730009)
    for n in range(2, 25):
        values = [rng.randint(-100, 100) for _ in range(n)]
        queries = [(rng.randrange(n), rng.randrange(n)) for _ in range(25)]
        cases.append((values, queries))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
