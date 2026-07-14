#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, operations):
    values = []
    expected = []
    for operation in operations:
        if operation[0] == 1:
            values.append(operation[1])
        elif operation[0] == 2:
            values.remove(operation[1])
        else:
            expected.append(min(values[i] ^ values[j] for i in range(len(values))
                                for j in range(i + 1, len(values))))
    lines = [str(len(operations))]
    lines += [str(op[0]) if op[0] == 3 else f"{op[0]} {op[1]}" for op in operations]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, expected)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[(1, 0), (1, (1 << 30) - 1), (3,)],
             [(1, 7), (1, 7), (3,), (2, 7), (1, 3), (3,)],
             [(1, 1), (1, 2), (1, 4), (3,), (2, 2), (3,)]]
    rng = random.Random(30807)
    for _ in range(9):
        values = []
        operations = []
        for step in range(60):
            choice = rng.randrange(3)
            if len(values) < 2 or choice == 0:
                value = rng.randrange(1 << 16)
                values.append(value)
                operations.append((1, value))
            elif choice == 1:
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
