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
    rng = random.Random(0xA660)
    queue: deque[str] = deque()
    operations: list[str] = []
    outputs: list[str] = []
    for _ in range(2000):
        choice = rng.randrange(4)
        if not queue or choice == 0:
            value = "".join(rng.choices(string.ascii_letters, k=rng.randrange(1, 6)))
            operations.append(f"0 {value}")
            queue.append(value)
        elif choice == 1:
            operations.append("1")
            queue.popleft()
        elif choice == 2:
            operations.append("2")
            outputs.append(queue[0])
        else:
            operations.append("3")
            outputs.append("".join(queue) if queue else "-")
    (args.out_dir / "case_00.in").write_text(
        str(len(operations)) + "\n" + "\n".join(operations) + "\n",
        encoding="utf-8",
    )
    (args.out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
