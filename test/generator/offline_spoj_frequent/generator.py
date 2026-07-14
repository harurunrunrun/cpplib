#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import collections
import random
from pathlib import Path


def build_case(values, query_count, rng):
    queries = []
    for _ in range(query_count):
        left = rng.randrange(len(values))
        right = rng.randrange(left, len(values))
        queries.append((left, right))
    lines = [f"{len(values)} {query_count}", " ".join(map(str, values))]
    lines.extend(f"{left + 1} {right + 1}" for left, right in queries)
    answers = [max(collections.Counter(values[left:right + 1]).values()) for left, right in queries]
    return lines, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76016)
    random_values = sorted(rng.randrange(-10, 11) for _ in range(89))
    cases = [build_case([7], 5, rng), build_case(random_values, 160, rng)]
    input_lines = []
    output_lines = []
    for lines, answers in cases:
        input_lines.extend(lines)
        output_lines.extend(map(str, answers))
    input_lines.append("0")
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n")


if __name__ == "__main__":
    main()
