#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def values(maximum: int, modulus: int) -> list[int]:
    answer = [1 % modulus] + [0] * maximum
    for size in range(2, maximum + 1):
        answer[size] = (size - 1) * (answer[size - 1] + answer[size - 2]) % modulus
    return answer


def write_case(out_dir: Path, index: int,
               cases: list[tuple[int, int]]) -> None:
    input_data = str(len(cases)) + "\n" + "".join(
        f"{n} {m}\n" for n, m in cases
    )
    output_data = "".join(
        " ".join(map(str, values(n, m))) + "\n" for n, m in cases
    )
    (out_dir / f"case_{index:02d}.in").write_text(input_data, encoding="utf-8")
    (out_dir / f"case_{index:02d}.out").write_text(output_data, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_case(args.out_dir, 0, [(n, m) for n in range(31)
                                for m in [1, 2, 3, 4, 7, 1000000000]])
    rng = random.Random(2026071310)
    write_case(args.out_dir, 1, [
        (rng.randrange(1, 2000), rng.randrange(1, 10**9 + 1))
        for _ in range(100)
    ])
    write_case(args.out_dir, 2, [(100000, 998244353)])


if __name__ == "__main__":
    main()
