#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = []
    line = [(x, 2 * x + 1) for x in range(-7, 8)]
    cases.append((line + [(-8, 50), (0, -40), (9, 9), (20, -7)], 1e-12, 600, 77, len(line)))
    vertical = [(5, y) for y in range(-10, 11, 2)]
    cases.append((vertical + [(0, 0), (10, 1), (-3, 7)], 1e-12, 500, 912, len(vertical)))
    duplicate = [(3, 4)] * 12
    cases.append((duplicate, 0.0, 50, 3, 0))
    for case_id, (points, threshold, iterations, seed, expected) in enumerate(cases):
        lines = [f"{len(points)} {threshold} {iterations} {seed} {expected}"]
        lines.extend(f"{x} {y}" for x, y in points)
        (out_dir / f"case_{case_id:02}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
        (out_dir / f"case_{case_id:02}.out").write_text(f"{expected}\n", encoding="utf-8")


if __name__ == "__main__":
    main()
