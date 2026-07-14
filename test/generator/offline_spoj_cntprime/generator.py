#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def is_prime(value):
    if value < 2:
        return False
    divisor = 2
    while divisor * divisor <= value:
        if value % divisor == 0:
            return False
        divisor += 1
    return True


def build_case(values, operation_count, rng):
    current = values[:]
    lines = [f"{len(values)} {operation_count}", " ".join(map(str, values))]
    answers = []
    for step in range(operation_count):
        left = rng.randrange(len(values))
        right = rng.randrange(left, len(values))
        if step % 3 == 0:
            value = rng.randrange(0, 300)
            lines.append(f"0 {left + 1} {right + 1} {value}")
            current[left:right + 1] = [value] * (right - left + 1)
        else:
            lines.append(f"1 {left + 1} {right + 1}")
            answers.append(str(sum(is_prime(value) for value in current[left:right + 1])))
    return lines, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76008)
    cases = [build_case([2], 12, rng),
             build_case([0, 1, 2, 3, 4, 999983, 1000000], 30, rng),
             build_case([rng.randrange(0, 500) for _ in range(37)], 100, rng)]
    input_lines = [str(len(cases))]
    output_lines = []
    for index, (lines, answers) in enumerate(cases, 1):
        input_lines.extend(lines)
        output_lines.append(f"Case {index}:")
        output_lines.extend(answers)
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n")


if __name__ == "__main__":
    main()
