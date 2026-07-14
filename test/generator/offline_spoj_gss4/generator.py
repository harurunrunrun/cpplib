#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import math
import random
from pathlib import Path


def build_case(values, operation_count, rng):
    current = values[:]
    lines = [str(len(values)), " ".join(map(str, values)), str(operation_count)]
    answers = []
    for step in range(operation_count):
        left = rng.randrange(len(values))
        right = rng.randrange(len(values))
        low, high = min(left, right), max(left, right)
        if step % 3 != 2:
            lines.append(f"0 {left + 1} {right + 1}")
            for index in range(low, high + 1):
                current[index] = math.isqrt(current[index])
        else:
            lines.append(f"1 {left + 1} {right + 1}")
            answers.append(str(sum(current[low:high + 1])))
    return lines, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76023)
    cases = [build_case([0], 12, rng),
             build_case([0, 1, 2, 3, 10**12], 30, rng),
             build_case([rng.randrange(0, 10**10) for _ in range(53)], 160, rng)]
    input_lines = []
    output_lines = []
    for index, (lines, answers) in enumerate(cases, 1):
        input_lines.extend(lines)
        output_lines.append(f"Case #{index}:")
        output_lines.extend(answers)
        output_lines.append("")
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n")


if __name__ == "__main__":
    main()
