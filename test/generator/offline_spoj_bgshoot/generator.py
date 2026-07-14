#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, intervals, queries):
    lines = [str(len(intervals))]
    lines.extend(f"{left} {right}" for left, right in intervals)
    lines.append(str(len(queries)))
    lines.extend(f"{left} {right}" for left, right in queries)
    answers = []
    normalized = [(min(a, b), max(a, b)) for a, b in intervals]
    for a, b in queries:
        left, right = min(a, b), max(a, b)
        answers.append(max(sum(x <= point <= y for x, y in normalized) for point in range(left, right + 1)))
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answers)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, 0, [(0, 0)], [(0, 0), (1, 1)])
    rng = random.Random(76005)
    intervals = [(rng.randrange(-20, 21), rng.randrange(-20, 21)) for _ in range(55)]
    queries = [(rng.randrange(-20, 21), rng.randrange(-20, 21)) for _ in range(70)]
    write_case(out_dir, 1, intervals, queries)


if __name__ == "__main__":
    main()
