#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encode(value: str) -> str:
    return value if value else "-"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    operations: list[tuple[int, str]] = [
        (2, ""),
        (3, ""),
        (0, ""),
        (0, "abc"),
        (0, "abc"),
        (2, "abc"),
        (3, "ab"),
        (1, "abc"),
        (1, "dddddddd"),
        (1, ""),
    ]
    known = ["", "a", "ab", "abc", "abcd", "b", "ba", "cab"]
    for _ in range(4490):
        value = rng.choice(known) if rng.randrange(4) == 0 else "".join(rng.choice("abcd") for _ in range(rng.randrange(11)))
        operations.append((rng.choices([0, 1, 2, 3], weights=[4, 2, 2, 2])[0], value))

    counts: dict[str, int] = {}
    total = 0
    input_lines = [str(len(operations))]
    output_lines: list[str] = []
    for operation, value in operations:
        input_lines.append(f"{operation} {encode(value)}")
        count = counts.get(value, 0)
        if operation == 0:
            counts[value] = count + 1
            total += 1
            output_lines.append(str(total))
        elif operation == 1:
            erased = count > 0
            if erased:
                counts[value] = count - 1
                total -= 1
            output_lines.append(f"{int(erased)} {total}")
        elif operation == 2:
            output_lines.append(f"{count} {int(count > 0)} {total}")
        else:
            prefix_count = sum(amount for key, amount in counts.items() if key.startswith(value))
            output_lines.append(f"{prefix_count} {total}")

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
