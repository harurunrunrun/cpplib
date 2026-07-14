#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def first_fit(capacity, items):
    remaining = []
    for item in items:
        for index, space in enumerate(remaining):
            if space >= item:
                remaining[index] -= item
                break
        else:
            remaining.append(capacity - item)
    return len(remaining), sum(remaining)


def encode_items(items):
    result = []
    index = 0
    while index < len(items):
        end = index + 1
        while end < len(items) and items[end] == items[index]:
            end += 1
        repeat = end - index
        if repeat >= 2:
            result.append(f"b {repeat} {items[index]}")
        else:
            result.append(str(items[index]))
        index = end
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20202)
    cases = [
        (10, [10]),
        (10, [6, 6, 4, 4]),
        (7, [1] * 8 + [7, 3, 4, 3]),
    ]
    for capacity in (5, 11, 23):
        items = [rng.randint(1, capacity) for _ in range(45)]
        items[10:15] = [capacity // 2 or 1] * 5
        cases.append((capacity, items))

    lines = [str(len(cases))]
    answers = []
    for capacity, items in cases:
        lines.append(f"{capacity} {len(items)}")
        lines.extend(encode_items(items))
        bins, waste = first_fit(capacity, items)
        answers.append(f"{bins} {waste}")
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
