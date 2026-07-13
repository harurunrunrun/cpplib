#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def pack(
    items: list[int],
    capacity: int,
    method: str,
    decreasing: bool,
) -> tuple[list[int], list[int]]:
    order = list(range(len(items)))
    if decreasing:
        order.sort(key=lambda item: -items[item])
    assignment = [0] * len(items)
    remaining: list[int] = []
    for item in order:
        size = items[item]
        bin_id: int | None = None
        if method == "next":
            if remaining and remaining[-1] >= size:
                bin_id = len(remaining) - 1
        elif method == "first":
            bin_id = next(
                (index for index, value in enumerate(remaining) if value >= size),
                None,
            )
        elif method == "best":
            candidates = [
                (value, index)
                for index, value in enumerate(remaining)
                if value >= size
            ]
            if candidates:
                bin_id = min(candidates)[1]
        elif method == "worst":
            candidates = [
                (value, index)
                for index, value in enumerate(remaining)
                if value >= size
            ]
            if candidates:
                bin_id = max(candidates)[1]
        else:
            raise ValueError(method)
        if bin_id is None:
            bin_id = len(remaining)
            remaining.append(capacity)
        assignment[item] = bin_id
        remaining[bin_id] -= size
    return assignment, remaining


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071313)
    cases: list[tuple[list[int], int]] = [
        ([], 10),
        ([0], 10),
        ([10], 10),
        ([6, 4, 6, 4, 5, 5], 10),
        ([2, 5, 4, 7, 1, 3, 8], 10),
        ([1] * 40, 7),
    ]
    for _ in range(54):
        capacity = rng.randrange(1, 10_001)
        size = rng.randrange(0, 100)
        cases.append(([rng.randrange(0, capacity + 1) for _ in range(size)], capacity))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for items, capacity in cases:
        input_lines.extend([
            f"{len(items)} {capacity}",
            " ".join(map(str, items)),
        ])
        for decreasing in (False, True):
            for method in ("next", "first", "best", "worst"):
                assignment, remaining = pack(items, capacity, method, decreasing)
                output_lines.append(
                    " ".join(map(str, [len(remaining), *assignment]))
                )

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
