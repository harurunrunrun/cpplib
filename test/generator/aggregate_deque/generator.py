#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from collections import deque
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(0xDE0A)
    values: deque[str] = deque()
    operations: list[str] = []
    outputs: list[str] = []
    for _ in range(4000):
        choice = rng.randrange(7)
        if not values and choice in (2, 3, 4, 5):
            choice = rng.randrange(2)
        if choice in (0, 1):
            value = "".join(rng.choices(string.ascii_letters, k=rng.randrange(1, 5)))
            operations.append(f"{choice} {value}")
            if choice == 0:
                values.appendleft(value)
            else:
                values.append(value)
        elif choice == 2:
            operations.append("2")
            values.popleft()
        elif choice == 3:
            operations.append("3")
            values.pop()
        elif choice == 4:
            operations.append("4")
            outputs.append(values[0])
        elif choice == 5:
            operations.append("5")
            outputs.append(values[-1])
        else:
            operations.append("6")
            outputs.append("".join(values) if values else "-")
    (args.out_dir / "case_00.in").write_text(
        str(len(operations)) + "\n" + "\n".join(operations) + "\n",
        encoding="utf-8",
    )
    (args.out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
