#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Operation = tuple[int, int, int | None]


def write_case(out_dir: Path, case_index: int, operations: list[Operation]) -> None:
    states: list[list[int]] = [[]]
    answers: list[int] = []
    for kind, base_query, value in operations:
        current = states[base_query + 1][:]
        if kind == 0:
            assert value is not None
            current.append(value)
        else:
            assert current
            answers.append(current.pop(0))
        states.append(current)

    stem = f"case_{case_index:03d}"
    lines = [str(len(operations))]
    for kind, base_query, value in operations:
        if kind == 0:
            lines.append(f"0 {base_query} {value}")
        else:
            lines.append(f"1 {base_query}")
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="ascii")
    (out_dir / f"{stem}.out").write_text(
        "\n".join(map(str, answers)) + ("\n" if answers else ""),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, [])
    write_case(
        out_dir,
        1,
        [
            (0, -1, -(2**31)),
            (0, 0, 2**31 - 1),
            (1, 1, None),
            (0, 0, 7),
            (1, 3, None),
            (1, 2, None),
        ],
    )

    rng = random.Random(2026071502)
    for case_index in range(2, 42):
        operation_count = rng.randrange(1, 240)
        operations: list[Operation] = []
        states: list[list[int]] = [[]]
        for query in range(operation_count):
            base_query = rng.randrange(-1, query)
            base = states[base_query + 1][:]
            if base and rng.randrange(2):
                operations.append((1, base_query, None))
                base.pop(0)
            else:
                value = rng.randrange(-(2**31), 2**31)
                operations.append((0, base_query, value))
                base.append(value)
            states.append(base)
        write_case(out_dir, case_index, operations)


if __name__ == "__main__":
    main()
