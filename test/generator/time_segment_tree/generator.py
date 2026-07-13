#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import Counter
from pathlib import Path


def write_case(
    out_dir: Path,
    case_id: int,
    time_count: int,
    intervals: list[tuple[int, int, int]],
) -> None:
    input_lines = [f"{time_count} {len(intervals)}"]
    input_lines.extend(f"{left} {right} {value}" for left, right, value in intervals)

    output_lines: list[str] = []
    for time in range(time_count):
        active = [value for left, right, value in intervals if left <= time < right]
        output_lines.append(f"{len(active)} {len(Counter(active))} {sum(active)}")

    (out_dir / f"case_{case_id:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{case_id:02d}.out").write_text(
        "\n".join(output_lines) + ("\n" if output_lines else ""), encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, 0, [])
    write_case(
        out_dir,
        1,
        8,
        [
            (0, 8, 4),
            (0, 1, -9),
            (1, 7, 4),
            (2, 2, 100),
            (3, 8, -2),
            (7, 8, 11),
        ],
    )

    rng = random.Random(2026071302)
    for case_id, (time_count, interval_count) in enumerate(
        [(1, 80), (31, 450), (256, 1400), (701, 3500)], start=2
    ):
        intervals: list[tuple[int, int, int]] = []
        for index in range(interval_count):
            if index % 23 == 0:
                left, right = 0, time_count
            elif index % 29 == 0:
                left = right = rng.randrange(time_count + 1)
            else:
                left = rng.randrange(time_count + 1)
                right = rng.randrange(time_count + 1)
                if right < left:
                    left, right = right, left
            value = rng.randrange(-10**9, 10**9 + 1)
            intervals.append((left, right, value))
        write_case(out_dir, case_id, time_count, intervals)


if __name__ == "__main__":
    main()
