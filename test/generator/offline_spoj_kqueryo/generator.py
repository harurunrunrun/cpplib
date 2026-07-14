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
    rng = random.Random(15016)
    values = [rng.randrange(0, 101) for _ in range(50)]
    desired = [(1, len(values), 0), (0, len(values) + 4, 50), (20, 10, 30)]
    for _ in range(100):
        desired.append((rng.randrange(0, len(values) + 3), rng.randrange(0, len(values) + 3), rng.randrange(0, 121)))
    encoded = []
    answers = []
    last = 0
    for left, right, threshold in desired:
        encoded.append((left ^ last, right ^ last, threshold ^ last))
        clamped_left = max(left, 1)
        clamped_right = min(right, len(values))
        if clamped_left > clamped_right:
            last = 0
        else:
            last = sum(value > threshold for value in values[clamped_left - 1 : clamped_right])
        answers.append(str(last))
    lines = [str(len(values)), " ".join(map(str, values)), str(len(encoded))]
    lines.extend(" ".join(map(str, query)) for query in encoded)
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
