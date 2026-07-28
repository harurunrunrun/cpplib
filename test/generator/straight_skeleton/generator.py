#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
import argparse
import math
import random
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True, type=Path)
args = parser.parse_args()
args.out_dir.mkdir(parents=True, exist_ok=True)

cases = [
    (
        [
            (0, 0), (12, 0), (12, 12), (9, 12),
            (9, 3), (7, 3), (7, 10), (5, 10),
            (5, 3), (3, 3), (3, 12), (0, 12),
        ],
        0,
        1,
    ),
    ([(0, 0), (4, 0), (0, 3)], 0, 0),
    (
        [
            (7.940774933223, 0.458863475953),
            (10.720524418796, 7.798776327457),
            (3.998582926933, 10.288582560732),
            (-1.341075038069, 6.598067021531),
            (-9.665286281287, 9.933379305365),
            (-17.336325221796, 4.020396408206),
            (-16.182054467885, -5.784723573162),
            (-10.075866731591, -13.082886703239),
            (-0.877538812568, -10.289607104365),
            (7.166221910348, -13.561724735513),
            (10.305636728542, -5.811685397577),
        ],
        1,
        0,
    ),
    ([(0, 0), (4, 0), (4, 4), (0, 4)], 0, 0),
    ([(0, 0), (8, 0), (8, 2), (0, 2)], 0, 1),
    ([(0, 0), (6, 0), (6, 2), (2, 2), (2, 6), (0, 6)], 0, 0),
    (
        [
            (0, 0), (20, 0), (20, 10), (12, 10),
            (12, 3), (8, 3), (8, 10), (0, 10),
        ],
        2,
        1,
    ),
    (
        list(reversed([
            (0, 0), (20, 0), (20, 10), (12, 10),
            (12, 3), (8, 3), (8, 10), (0, 10),
        ])),
        2,
        1,
    ),
    (
        [
            (0, 0), (24, 0), (24, 10), (20, 10),
            (20, 4), (17, 4), (17, 10), (13, 10),
            (13, 3), (10, 3), (10, 10), (6, 10),
            (6, 5), (3, 5), (3, 10), (0, 10),
        ],
        2,
        0,
    ),
]

randomizer = random.Random(20260729)
templates = [
    ([(0, 0), (8, 0), (8, 2), (0, 2)], 0, 1),
    ([(0, 0), (6, 0), (6, 2), (2, 2), (2, 6), (0, 6)], 0, 0),
    (
        [
            (0, 0), (20, 0), (20, 10), (12, 10),
            (12, 3), (8, 3), (8, 10), (0, 10),
        ],
        2,
        1,
    ),
]
for case_index in range(30):
    template, minimum_splits, simultaneous = templates[case_index % 3]
    scale = randomizer.randint(1, 100)
    shift_x = randomizer.randint(-10000, 10000)
    shift_y = randomizer.randint(-10000, 10000)
    polygon = [
        (shift_x + scale * x, shift_y + scale * y)
        for x, y in template
    ]
    if case_index % 2 != 0:
        polygon.reverse()
    cases.append((polygon, minimum_splits, simultaneous))
for case_index in range(40):
    size = 7 + case_index % 12
    phase = randomizer.uniform(-0.1, 0.1)
    polygon = []
    for index in range(size):
        angle = phase + 2.0 * math.pi * index / size
        radius = randomizer.uniform(5.0, 18.0)
        polygon.append((
            round(radius * math.cos(angle), 12),
            round(radius * math.sin(angle), 12),
        ))
    cases.append((polygon, 0, 0))

lines = [str(len(cases))]
for polygon, minimum_splits, simultaneous in cases:
    lines.append(f"{len(polygon)} {minimum_splits} {simultaneous}")
    lines.extend(f"{x} {y}" for x, y in polygon)
(args.out_dir / "case_00.in").write_text(
    "\n".join(lines) + "\n", encoding="utf-8"
)
(args.out_dir / "case_00.out").write_text(
    "OK\n" * len(cases), encoding="utf-8"
)
