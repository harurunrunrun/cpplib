#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


Case = tuple[int, int, int, list[tuple[int, int]]]


def write_case(path: Path, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for index, (width, height, expected_bins, rectangles) in enumerate(cases):
        input_lines.append(
            f"{len(rectangles)} {width} {height} {expected_bins}"
        )
        input_lines.extend(f"{w} {h}" for w, h in rectangles)
        output_lines.append(
            f"OK {index} {len(rectangles)} {width} {height}"
        )
    path.with_suffix(".in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    path.with_suffix(".out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    deterministic: list[Case] = [
        (10, 10, 0, []),
        (10, 10, 1, [(10, 10)]),
        (10, 10, 1, [(5, 10), (5, 10)]),
        (10, 10, 1, [(10, 5), (10, 5)]),
        (10, 10, -1, [(6, 4), (4, 6), (4, 4), (6, 6)]),
        (13, 11, -1, [(8, 3), (5, 8), (3, 8), (5, 3), (2, 11)]),
        (17, 13, -1, [(1, 13)] * 17),
        (17, 13, -1, [(17, 1)] * 13),
        (20, 15, -1, [(11, 4), (9, 4), (5, 7), (4, 11), (8, 3)]),
        (7, 9, -1, [(7, 9), (1, 1), (6, 8), (2, 7)]),
    ]
    write_case(out_dir / "case_00", deterministic)

    rng = random.Random(2026071321)
    random_cases: list[Case] = []
    for _ in range(30):
        width = rng.randrange(2, 61)
        height = rng.randrange(2, 61)
        count = rng.randrange(1, 15)
        rectangles = [
            (rng.randrange(1, width + 1), rng.randrange(1, height + 1))
            for _ in range(count)
        ]
        random_cases.append((width, height, -1, rectangles))
    write_case(out_dir / "case_01", random_cases)

    large = 9_000_000_000_000_000_000
    extreme_cases: list[Case] = [
        (large, large, -1, [(large - 1, large), (1, large)]),
        (large, large, -1, [(large, large // 2), (large, large - large // 2)]),
        (large, large, -1, [(large // 3, large // 3)] * 9),
        (large, 17, -1, [(large // 2, 17), (large - large // 2, 17)]),
        (23, large, -1, [(23, large // 4)] * 4),
    ]
    write_case(out_dir / "case_02", extreme_cases)


if __name__ == "__main__":
    main()
