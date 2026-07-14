#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, groups):
    lines = [str(len(groups))]
    output = []
    for n, updates, queries in groups:
        values = [0] * n
        lines.append(f"{n} {len(updates)}")
        for left, right, value in updates:
            lines.append(f"{left} {right} {value}")
            for position in range(left, right + 1):
                values[position] += value
        lines.append(str(len(queries)))
        lines += [str(position) for position in queries]
        output += [str(values[position]) for position in queries]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(output) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[(1, [(0, 0, -5), (0, 0, 8)], [0, 0])]]
    rng = random.Random(730021)
    for _ in range(10):
        groups = []
        for _ in range(rng.randint(1, 4)):
            n = rng.randint(1, 28)
            updates = []
            for _ in range(rng.randint(1, 30)):
                left = rng.randrange(n)
                right = rng.randrange(left, n)
                updates.append((left, right, rng.randint(-30, 30)))
            queries = [rng.randrange(n) for _ in range(30)]
            groups.append((n, updates, queries))
        cases.append(groups)
    for index, groups in enumerate(cases):
        write_case(out_dir, index, groups)


if __name__ == "__main__":
    main()
