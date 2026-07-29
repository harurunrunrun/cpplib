#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    arguments = parser.parse_args()
    arguments.out_dir.mkdir(parents=True, exist_ok=True)

    random_engine = random.Random(0xA110CA7E)
    batches = [[0, 1, 2, 2**64 - 1]]
    for _ in range(5):
        batches.append([random_engine.getrandbits(64) for _ in range(8)])
    for index, seeds in enumerate(batches):
        input_text = f"{len(seeds)}\n" + "".join(f"{seed}\n" for seed in seeds)
        output_text = "0\n" * len(seeds)
        (arguments.out_dir / f"{index}.in").write_text(
            input_text, encoding="utf-8"
        )
        (arguments.out_dir / f"{index}.out").write_text(
            output_text, encoding="utf-8"
        )


if __name__ == "__main__":
    main()
