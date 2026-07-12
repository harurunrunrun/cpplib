#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def rectangle_sum(a: list[list[int]], y1: int, x1: int, y2: int, x2: int) -> int:
    return sum(sum(row[x1:x2]) for row in a[y1:y2])


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    h, w = 17, 19
    values = [[rng.randrange(-100, 101) for _ in range(w)] for _ in range(h)]
    initial = [row[:] for row in values]
    commands = ["PREFIX 0 0", f"SUM 0 0 {h} {w}", "GET 0 0"]
    outputs = ["0", str(rectangle_sum(values, 0, 0, h, w)), str(values[0][0])]

    for _ in range(500):
        kind = rng.randrange(5)
        if kind == 0:
            y, x = rng.randrange(h), rng.randrange(w)
            value = rng.randrange(-100, 101)
            commands.append(f"ADD {y} {x} {value}")
            values[y][x] += value
        elif kind == 1:
            y, x = rng.randrange(h), rng.randrange(w)
            value = rng.randrange(-300, 301)
            commands.append(f"SET {y} {x} {value}")
            values[y][x] = value
        elif kind == 2:
            y, x = rng.randrange(h), rng.randrange(w)
            commands.append(f"GET {y} {x}")
            outputs.append(str(values[y][x]))
        elif kind == 3:
            y1, y2 = sorted((rng.randrange(h + 1), rng.randrange(h + 1)))
            x1, x2 = sorted((rng.randrange(w + 1), rng.randrange(w + 1)))
            commands.append(f"SUM {y1} {x1} {y2} {x2}")
            outputs.append(str(rectangle_sum(values, y1, x1, y2, x2)))
        else:
            y, x = rng.randrange(h + 1), rng.randrange(w + 1)
            commands.append(f"PREFIX {y} {x}")
            outputs.append(str(rectangle_sum(values, 0, 0, y, x)))

    lines = [f"{h} {w} {len(commands)}"]
    lines.extend(" ".join(map(str, row)) for row in initial)
    lines.extend(commands)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
