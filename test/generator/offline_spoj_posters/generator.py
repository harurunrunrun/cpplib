#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def visible_count(intervals):
    coordinates = sorted({coordinate for interval in intervals for coordinate in interval})
    coordinate_id = {coordinate: index for index, coordinate in enumerate(coordinates)}
    # Even atoms are endpoints; odd atoms are the open gaps between neighbours.
    top = [0] * (2 * len(coordinates) - 1)
    for poster, (a, b) in enumerate(intervals, 1):
        first = 2 * coordinate_id[a]
        last = 2 * coordinate_id[b]
        for atom in range(first, last + 1):
            top[atom] = poster
    return len(set(top) - {0})


def write_case(out_dir, index, groups):
    lines = [str(len(groups))]
    answers = []
    for intervals in groups:
        lines.append(str(len(intervals)))
        lines += [f"{left} {right}" for left, right in intervals]
        answers.append(visible_count(intervals))
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answers)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        [[(0, 0)]],
        [[(0, 2), (3, 5), (6, 9)]],
        [[(0, 10), (1, 9), (2, 8), (3, 7)]],
        [[(1, 4), (2, 6), (8, 10), (3, 9)]],
    ]
    rng = random.Random(730001)
    for _ in range(8):
        groups = []
        for _ in range(rng.randint(1, 4)):
            intervals = []
            for _ in range(rng.randint(1, 18)):
                left = rng.randint(0, 30)
                right = rng.randint(left, 35)
                intervals.append((left, right))
            groups.append(intervals)
        cases.append(groups)
    # Maximum number of distinct endpoints/atoms allowed by the SPOJ input.
    cases.append([[(2 * index, 2 * index) for index in range(40000)]])
    for index, groups in enumerate(cases):
        write_case(out_dir, index, groups)


if __name__ == "__main__":
    main()
