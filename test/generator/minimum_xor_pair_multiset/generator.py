#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, operations):
    values = []
    answer = []
    for operation in operations:
        if operation[0] == 1:
            values.append(operation[1])
        elif operation[0] == 2:
            values.remove(operation[1])
        else:
            answer.append(min(
                values[left] ^ values[right]
                for left in range(len(values))
                for right in range(left + 1, len(values))
            ))
    lines = [str(len(operations))]
    lines.extend(str(op[0]) if op[0] == 3 else f"{op[0]} {op[1]}" for op in operations)
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(map(str, answer)) + ("\n" if answer else ""), encoding="utf-8"
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        [(1, 0), (1, (1 << 32) - 1), (3,)],
        [(1, 9), (1, 9), (3,), (2, 9), (1, 1), (3,)],
    ]
    rng = random.Random(3080701)
    for _ in range(25):
        values = []
        operations = []
        for _ in range(100):
            choice = rng.randrange(4)
            if len(values) < 2 or choice <= 1:
                value = rng.randrange(1 << 32)
                values.append(value)
                operations.append((1, value))
            elif choice == 2:
                value = rng.choice(values)
                values.remove(value)
                operations.append((2, value))
            else:
                operations.append((3,))
        if len(values) >= 2:
            operations.append((3,))
        cases.append(operations)
    for index, operations in enumerate(cases):
        write_case(out_dir, index, operations)


if __name__ == "__main__":
    main()
