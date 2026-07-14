#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


def solve(size):
    positions = list(range(size))
    answer = [0] * size
    cursor = 0
    for card in range(1, size + 1):
        cursor = (cursor + card) % len(positions)
        position = positions.pop(cursor)
        answer[position] = card
        if positions:
            cursor %= len(positions)
    return answer


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    sizes = [1, 2, 3, 10, 37, 101]
    input_text = str(len(sizes)) + "\n" + "\n".join(map(str, sizes)) + "\n"
    output_text = "\n".join(" ".join(map(str, solve(size))) for size in sizes) + "\n"
    (out_dir / "case_00.in").write_text(input_text)
    (out_dir / "case_00.out").write_text(output_text)


if __name__ == "__main__":
    main()
