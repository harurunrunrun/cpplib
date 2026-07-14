#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, heights, throws):
    successful = 0
    for source, target in throws:
        left = min(source, target)
        right = max(source, target)
        if right - left <= 1 or max(heights[left + 1:right]) <= heights[source]:
            successful += 1
    lines = [f"{len(heights)} {len(throws)}", " ".join(map(str, heights))]
    lines += [f"{source + 1} {target + 1}" for source, target in throws]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{successful}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([5], [(0, 0)]), ([5, 1, 4, 6], [(0, 2), (2, 0), (3, 0), (1, 2)])]
    rng = random.Random(730016)
    for n in range(2, 30):
        heights = [rng.randint(0, 100) for _ in range(n)]
        throws = [(rng.randrange(n), rng.randrange(n)) for _ in range(40)]
        cases.append((heights, throws))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
