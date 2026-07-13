#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071302)
    first = [rng.getrandbits(63) for _ in range(3500)]
    second = first[1400:] + [rng.getrandbits(63) for _ in range(1400)]
    rng.shuffle(first)
    rng.shuffle(second)
    lines = [f"{len(first)} {len(second)}", " ".join(map(str, first)), " ".join(map(str, second))]
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
