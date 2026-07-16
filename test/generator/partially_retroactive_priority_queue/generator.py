#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path


Command = tuple[str, int, int]


def replay(history: dict[int, tuple[str, int]]) -> list[int] | None:
    heap: list[int] = []
    for time in sorted(history):
        kind, value = history[time]
        if kind == "P":
            heapq.heappush(heap, value)
        else:
            if not heap:
                return None
            heapq.heappop(heap)
    return sorted(heap)


def write_case(out_dir: Path, index: int, commands: list[Command]) -> None:
    history: dict[int, tuple[str, int]] = {}
    current: list[int] = []
    input_lines = [str(len(commands))]
    output_lines: list[str] = []
    for kind, time, value in commands:
        if kind == "P":
            input_lines.append(f"P {time} {value}")
        else:
            input_lines.append(f"{kind} {time}")

        candidate = history.copy()
        structural_valid = True
        if kind in ("P", "O"):
            if time in candidate:
                structural_valid = False
            else:
                candidate[time] = (kind, value)
        elif time not in candidate:
            structural_valid = False
        else:
            del candidate[time]
        replayed = replay(candidate) if structural_valid else None
        success = replayed is not None
        if success:
            history = candidate
            current = replayed
        state = "EMPTY" if not current else str(current[0])
        output_lines.append(
            f"{'OK' if success else 'ERROR'} {len(current)} {state}"
        )

    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output_lines) + ("\n" if output_lines else ""),
        encoding="utf-8",
    )


def make_valid_prefix(
    generator: random.Random, count: int, time_offset: int = 0
) -> list[Command]:
    commands: list[Command] = []
    size = 0
    for index in range(count):
        time = time_offset + index * 2
        if size == 0 or generator.randrange(3) != 0:
            commands.append(("P", time, generator.randrange(-10**9, 10**9 + 1)))
            size += 1
        else:
            commands.append(("O", time, 0))
            size -= 1
    return commands


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    cases: list[list[Command]] = [
        [],
        [("O", 0, 0), ("P", 10, 5), ("O", 20, 0), ("E", 10, 0)],
        [
            ("P", 10, 5),
            ("P", 30, 2),
            ("O", 40, 0),
            ("P", 20, 1),
            ("O", 0, 0),
            ("P", 10, 99),
            ("E", 40, 0),
            ("E", 99, 0),
        ],
    ]

    generator = random.Random(2026071616)
    for _ in range(30):
        commands = make_valid_prefix(generator, generator.randrange(10, 80))
        known_times = [command[1] for command in commands]
        for _ in range(150):
            kind = generator.randrange(3)
            if kind == 0:
                commands.append(
                    (
                        "P",
                        generator.randrange(-100, 300),
                        generator.randrange(-10**6, 10**6 + 1),
                    )
                )
            elif kind == 1:
                commands.append(("O", generator.randrange(-100, 300), 0))
            else:
                if known_times and generator.randrange(2) == 0:
                    time = generator.choice(known_times)
                else:
                    time = generator.randrange(-100, 300)
                commands.append(("E", time, 0))
        cases.append(commands)

    large = make_valid_prefix(generator, 1500)
    for _ in range(500):
        large.append(
            (
                "P" if generator.randrange(2) == 0 else "E",
                generator.randrange(0, 3500),
                generator.randrange(-10**9, 10**9 + 1),
            )
        )
    cases.append(large)

    for index, commands in enumerate(cases):
        write_case(args.out_dir, index, commands)


if __name__ == "__main__":
    main()
