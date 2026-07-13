#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path

MOD = 998244353


def partitions(maximum: int) -> list[int]:
    answer = [0] * (maximum + 1)
    answer[0] = 1
    for value in range(1, maximum + 1):
        total = 0
        index = 1
        while True:
            first = index * (3 * index - 1) // 2
            if first > value:
                break
            sign = 1 if index % 2 == 1 else -1
            total += sign * answer[value - first]
            second = index * (3 * index + 1) // 2
            if second <= value:
                total += sign * answer[value - second]
            index += 1
        answer[value] = total % MOD
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    cases = [0, 1, 2, 3, 4, 5, 10, 50, 200, 1000, 10000]
    (args.out_dir / "case_00.in").write_text(
        str(len(cases)) + "\n" + "".join(f"{n}\n" for n in cases),
        encoding="utf-8",
    )
    (args.out_dir / "case_00.out").write_text(
        "".join(" ".join(map(str, partitions(n))) + "\n" for n in cases),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
