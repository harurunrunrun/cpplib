#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def largest_rectangle(heights: list[int]) -> int:
    stack: list[int] = []
    answer = 0
    for right in range(len(heights) + 1):
        current = 0 if right == len(heights) else heights[right]
        while stack and heights[stack[-1]] >= current:
            height = heights[stack.pop()]
            left = 0 if not stack else stack[-1] + 1
            answer = max(answer, height * (right - left))
        if right < len(heights):
            stack.append(right)
    return answer


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    cases = [
        [],
        [7] * 1000,
        list(range(2500)),
        [rng.randrange(0, 10**9 + 1) for _ in range(5000)],
    ]
    for case_id, heights in enumerate(cases):
        input_text = f"{len(heights)}\n" + " ".join(map(str, heights)) + "\n"
        (out_dir / f"case_{case_id:02d}.in").write_text(input_text, encoding="utf-8")
        (out_dir / f"case_{case_id:02d}.out").write_text(
            f"{largest_rectangle(heights)}\n", encoding="utf-8"
        )


if __name__ == "__main__":
    main()
