#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_3d_common"))
from generator_utils import CASES


def generate() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, (seed, rounds) in enumerate(CASES):
        stem = f"random_{index:02d}"
        (out_dir / f"{stem}.in").write_text(
            f"{seed} {rounds}\n",
            encoding="utf-8",
        )
        (out_dir / f"{stem}.out").write_text("OK\n", encoding="utf-8")

    # 1,200 points on the moment curve are all hull vertices.  Their exact
    # 2N-4 facets give the C++ test a closed-form oracle and make the former
    # all-faces x all-points expansion prohibitively expensive.
    (out_dir / "performance_moment_curve.in").write_text(
        "0 9999\n",
        encoding="utf-8",
    )
    (out_dir / "performance_moment_curve.out").write_text(
        "OK\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    generate()
