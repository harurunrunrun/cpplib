#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def earliest(country_count, owners, requirements, showers):
    gathered = [0] * len(owners)
    answer = [None] * country_count
    for country in range(country_count):
        if requirements[country] <= 0:
            answer[country] = 0
    for index, (left, right, amount) in enumerate(showers, 1):
        positions = range(left, right + 1) if left <= right else list(range(left, len(owners))) + list(range(0, right + 1))
        for sector in positions:
            gathered[sector] += amount
        for country in range(country_count):
            if answer[country] is not None:
                continue
            total = sum(gathered[sector] for sector, owner in enumerate(owners) if owner == country)
            if total >= requirements[country]:
                answer[country] = index
    return ["NIE" if value is None else str(value) for value in answer]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(12120)
    country_count = 9
    sector_count = 27
    owners = [rng.randrange(country_count) for _ in range(sector_count)]
    owners[:country_count] = range(country_count)
    requirements = [0, 1, 20, 75, 150, 500, 40, 300, 1000]
    showers = []
    for index in range(35):
        left = rng.randrange(sector_count)
        right = rng.randrange(sector_count)
        amount = rng.randrange(0, 31)
        showers.append((left, right, amount))
    lines = [f"{country_count} {sector_count}", " ".join(str(owner + 1) for owner in owners), " ".join(map(str, requirements)), str(len(showers))]
    lines.extend(f"{left + 1} {right + 1} {amount}" for left, right, amount in showers)
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(earliest(country_count, owners, requirements, showers)) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
