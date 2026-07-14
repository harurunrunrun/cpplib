#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def completion_times(processing):
    remaining = processing[:]
    answer = [0] * len(remaining)
    elapsed = 0
    unfinished = len(remaining)
    while unfinished:
        for index in range(len(remaining)):
            if remaining[index] == 0:
                continue
            remaining[index] -= 1
            elapsed += 1
            if remaining[index] == 0:
                answer[index] = elapsed
                unfinished -= 1
    return answer


def write_case(out_dir, index, processing):
    lines = [str(len(processing)), " ".join(map(str, processing))]
    answer = completion_times(processing)
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answer)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[1], [1, 1, 1], [5, 1, 3, 1], [10, 9, 8, 7, 6]]
    rng = random.Random(730011)
    for n in range(2, 25):
        cases.append([rng.randint(1, 12) for _ in range(n)])
    for index, processing in enumerate(cases):
        write_case(out_dir, index, processing)


if __name__ == "__main__":
    main()
