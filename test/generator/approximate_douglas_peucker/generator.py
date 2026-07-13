#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


def distance2(point, first, last):
    dx = last[0] - first[0]
    dy = last[1] - first[1]
    denominator = dx * dx + dy * dy
    ratio = 0.0 if denominator == 0 else max(0.0, min(1.0, ((point[0] - first[0]) * dx + (point[1] - first[1]) * dy) / denominator))
    x = first[0] + ratio * dx
    y = first[1] + ratio * dy
    return (point[0] - x) ** 2 + (point[1] - y) ** 2


def simplify(points, tolerance):
    if len(points) <= 2:
        return list(range(len(points)))
    keep = {0, len(points) - 1}
    stack = [(0, len(points) - 1)]
    while stack:
        first, last = stack.pop()
        values = [(distance2(points[i], points[first], points[last]), i) for i in range(first + 1, last)]
        maximum, index = max(values, default=(-1.0, first), key=lambda value: value[0])
        if maximum > tolerance * tolerance:
            keep.add(index)
            stack.append((index, last))
            stack.append((first, index))
    return sorted(keep)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        (0.3, [(0, 0), (1, 0.1), (2, -0.1), (3, 4), (4, 4.1), (5, 4)]),
        (0.0, [(0, 0), (1, 0), (1, 0), (2, 0)]),
        (10.0, [(i, (i * i + 3 * i) % 11) for i in range(40)]),
    ]
    for case_id, (tolerance, points) in enumerate(cases):
        lines = [f"{len(points)} {tolerance}"] + [f"{x} {y}" for x, y in points]
        indices = simplify(points, tolerance)
        (out_dir / f"case_{case_id:02}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
        (out_dir / f"case_{case_id:02}.out").write_text(str(len(indices)) + " " + " ".join(map(str, indices)) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
