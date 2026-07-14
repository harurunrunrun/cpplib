#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    random_engine = random.Random(2026071401)
    size = 12
    initial = [random_engine.randrange(-1000, 1001) for _ in range(size)]
    per_index: list[list[tuple[int, int]]] = [[] for _ in range(size)]
    changes: list[tuple[int, int, int]] = []
    commands: list[str] = []
    answers: list[str] = []
    next_version = 0

    for _ in range(1200):
        operation = random_engine.randrange(10)
        if operation < 4:
            index = random_engine.randrange(size)
            next_version += random_engine.randrange(1, 4)
            value = random_engine.randrange(-10**9, 10**9 + 1)
            commands.append(f"WRITE {index} {next_version} {value}")
            per_index[index].append((next_version, value))
            changes.append((index, next_version, value))
        elif operation == 4 and changes:
            snapshot = random_engine.randrange(len(changes) + 1)
            commands.append(f"ROLLBACK {snapshot}")
            while len(changes) > snapshot:
                index, _, _ = changes.pop()
                per_index[index].pop()
        elif operation < 7:
            index = random_engine.randrange(size)
            commands.append(f"CURRENT {index}")
            value = per_index[index][-1][1] if per_index[index] else initial[index]
            answers.append(str(value))
        elif operation < 9:
            index = random_engine.randrange(size)
            version = random_engine.randrange(-3, next_version + 4)
            commands.append(f"GET {index} {version}")
            value = initial[index]
            for change_version, change_value in per_index[index]:
                if change_version <= version:
                    value = change_value
                else:
                    break
            answers.append(str(value))
        else:
            commands.append("CHANGES")
            answers.append(str(len(changes)))

    input_text = (
        f"{size} {len(commands)}\n"
        + " ".join(map(str, initial))
        + "\n"
        + "\n".join(commands)
        + "\n"
    )
    (out_dir / "case_00.in").write_text(input_text, encoding="utf-8")
    (out_dir / "case_00.out").write_text(
        "\n".join(answers) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
