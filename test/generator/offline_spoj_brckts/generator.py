#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def balanced(brackets):
    balance = 0
    for bracket in brackets:
        balance += 1 if bracket == "(" else -1
        if balance < 0:
            return False
    return balance == 0


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76006)
    input_lines = []
    output_lines = []
    for test in range(1, 11):
        size = test * 2 - (1 if test == 1 else 0)
        brackets = [rng.choice("()") for _ in range(size)]
        operations = [0]
        operations.extend(0 if step % 4 == 0 else rng.randrange(1, size + 1) for step in range(24))
        operations.append(0)
        input_lines.extend([str(size), "".join(brackets), str(len(operations))])
        output_lines.append(f"Test {test}:")
        for position in operations:
            input_lines.append(str(position))
            if position == 0:
                output_lines.append("YES" if balanced(brackets) else "NO")
            else:
                index = position - 1
                brackets[index] = ")" if brackets[index] == "(" else "("
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n")


if __name__ == "__main__":
    main()
