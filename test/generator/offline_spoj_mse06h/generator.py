#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def crossing_count(roads):
    return sum(
        (east_left - east_right) * (west_left - west_right) < 0
        for index, (east_left, west_left) in enumerate(roads)
        for east_right, west_right in roads[index + 1 :]
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(10060)
    cases = [(1, 1, []), (3, 3, [(1, 3), (2, 2), (3, 1)]), (4, 5, [(1, 2), (1, 4), (2, 1), (4, 5)])]
    for east_count, west_count, road_count in ((7, 8, 25), (12, 10, 55)):
        roads = [(rng.randint(1, east_count), rng.randint(1, west_count)) for _ in range(road_count)]
        cases.append((east_count, west_count, roads))
    lines = [str(len(cases))]
    answers = []
    for index, (east_count, west_count, roads) in enumerate(cases, 1):
        lines.append(f"{east_count} {west_count} {len(roads)}")
        lines.extend(f"{east} {west}" for east, west in roads)
        answers.append(f"Test case {index}: {crossing_count(roads)}")
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
