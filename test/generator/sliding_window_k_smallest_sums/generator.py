#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, values, window_size, k):
    answer = []
    for left in range(len(values) - window_size + 1):
        window = sorted(values[left:left + window_size])
        answer.append(sum(window[:k]))
    lines = [f"{len(values)} {window_size} {k}", " ".join(map(str, values))]
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        " ".join(map(str, answer)) + "\n", encoding="utf-8"
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        ([7], 1, 0),
        ([7], 1, 1),
        ([4, 1, 3, 2], 4, 2),
        ([-5, 8, -5, 3, 8, 0], 3, 3),
        ([2, 2, 2, 2, 2], 2, 1),
    ]
    rng = random.Random(2810501)
    for n in range(2, 42):
        values = [rng.randint(-10**6, 10**6) for _ in range(n)]
        window_size = rng.randint(1, n)
        k = rng.randint(0, window_size)
        cases.append((values, window_size, k))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
