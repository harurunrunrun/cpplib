#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    tree = [(index - 1, index) for index in range(1, 11)]
    cycle = [(index, (index + 1) % 13) for index in range(13)]
    parallel = [(0, 1)] * 7 + [(0, 0), (1, 1)]
    complete = [(first, second) for first in range(5) for second in range(first + 1, 5)]
    disconnected = [(0, 1), (1, 2), (3, 4), (4, 5)]
    cases = [
        (0, [], 0, 1),
        (1, [(0, 0)], 0, 1),
        (11, tree, 1, 1),
        (13, cycle, 2, 1),
        (2, parallel, 7, 1),
        (5, complete, 4, 0),
        (6, disconnected, 0, 1),
    ]
    lines = [str(len(cases))]
    for vertex_count, edges, expected, always in cases:
        lines.append(f"{vertex_count} {len(edges)} {expected} {always}")
        lines.extend(f"{first} {second}" for first, second in edges)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text(
        "\n".join(str(case[2]) for case in cases) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
