#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(
    0, str(Path(__file__).resolve().parents[1] / "geometry_2d_common")
)
from generator_utils import generate


def comb_case(teeth: int) -> tuple[str, str]:
    points = [(0, 0), (2 * teeth, 0), (2 * teeth, 3)]
    for tooth in range(teeth - 1, -1, -1):
        x = 2 * tooth
        points.extend(((x + 1, 3), (x + 1, 1), (x, 1)))
        if tooth:
            points.append((x, 3))
    tokens = [str(len(points))]
    for x, y in points:
        tokens.extend((str(x), str(y)))
    tokens.extend(("-1", "2", str(2 * teeth + 1), "2"))
    input_data = "1\n" + " ".join(tokens) + "\n"
    output_data = (
        f"{teeth} 1\n"
        f"{float(teeth):.10f}\n"
        f"{float(3 * teeth):.10f}\n"
    )
    return input_data, output_data


CASES = [
    (
        "3\n"
        "4 0 0 4 0 4 2 0 2 -1 1 5 1\n"
        "8 0 0 6 0 6 6 4 6 4 2 2 2 2 6 0 6 -1 4 7 4\n"
        "8 0 0 6 0 6 6 4 6 4 2 2 2 2 6 0 6 2 2 4 4\n",
        "1 1\n"
        "4.0000000000\n"
        "4.0000000000\n"
        "2 1\n"
        "8.0000000000\n"
        "20.0000000000\n"
        "2 1\n"
        "12.0000000000\n"
        "16.0000000000\n",
    ),
    comb_case(6000),
]


if __name__ == "__main__":
    generate(CASES)
