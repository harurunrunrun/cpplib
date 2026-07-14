#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def build_case(size, operation_count, rng):
    values = [0] * size
    lines = [f"{size} {operation_count}"]
    answers = []
    for step in range(operation_count):
        left = rng.randrange(size)
        right = rng.randrange(left, size)
        if step % 3 != 2:
            lines.append(f"modification {left + 1} {right + 1}")
            for index in range(left, right + 1):
                values[index] += 1
            answers.append("OK")
        else:
            lines.append(f"query {left + 1} {right + 1}")
            answers.append(str(sum(values[left:right + 1])))
    return lines, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76007)
    cases = [build_case(1, 8, rng), build_case(31, 100, rng)]
    input_lines = [str(len(cases))]
    output_lines = []
    for case_index, (lines, answers) in enumerate(cases, 1):
        input_lines.extend(lines)
        output_lines.append(f"Scenario #{case_index}:")
        output_lines.extend(answers)
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n")


if __name__ == "__main__":
    main()
