#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import deque
from pathlib import Path


MASK = (1 << 32) - 1
MAX_POWER = 10


def moments(values: list[int], left: int, right: int) -> list[int]:
    result = [0] * (MAX_POWER + 1)
    for position, value in enumerate(values[left:right], start=1):
        position_power = 1
        for power in range(MAX_POWER + 1):
            result[power] = (result[power] + value * position_power) & MASK
            position_power = position_power * position & MASK
    return result


def write_case(
    out_dir: Path,
    name: str,
    initial: list[int],
    commands: list[str],
    outputs: list[str],
) -> None:
    lines = [
        f"{len(initial)} {len(commands)}",
        " ".join(map(str, initial)),
        *commands,
    ]
    (out_dir / f"{name}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(outputs) + ("\n" if outputs else ""), encoding="utf-8"
    )


def small_random_case(rng: random.Random) -> tuple[list[int], list[str], list[str]]:
    initial = [0, 1, MASK, 1 << 31, MASK - 1]
    initial.extend(rng.randrange(1 << 32) for _ in range(35))
    values = initial[:]
    commands: list[str] = []
    outputs: list[str] = []
    edge_values = [0, 1, MASK, 1 << 31, MASK - 1]

    for step in range(1600):
        kind = rng.randrange(13)
        if kind == 0 and len(values) < 400:
            position = rng.randrange(len(values) + 1)
            value = rng.choice(edge_values) if step % 7 == 0 else rng.randrange(1 << 32)
            commands.append(f"INSERT {position} {value}")
            values.insert(position, value)
        elif kind == 1 and values:
            position = rng.randrange(len(values))
            commands.append(f"ERASE {position}")
            values.pop(position)
        elif kind == 2 and values:
            position = rng.randrange(len(values))
            value = rng.choice(edge_values) if step % 5 == 0 else rng.randrange(1 << 32)
            commands.append(f"SET {position} {value}")
            values[position] = value
        elif kind in {3, 4, 5}:
            left, right = sorted(
                (rng.randrange(len(values) + 1), rng.randrange(len(values) + 1))
            )
            commands.append(f"REVERSE {left} {right}")
            values[left:right] = reversed(values[left:right])
        elif kind in {6, 7, 8, 9, 10}:
            left, right = sorted(
                (rng.randrange(len(values) + 1), rng.randrange(len(values) + 1))
            )
            power = rng.randrange(MAX_POWER + 1)
            commands.append(f"QUERY {left} {right} {power}")
            outputs.append(str(moments(values, left, right)[power]))
        elif kind == 11:
            power = rng.randrange(MAX_POWER + 1)
            commands.append(f"ALL {power}")
            outputs.append(str(moments(values, 0, len(values))[power]))
        else:
            commands.append("SIZE")
            outputs.append(str(len(values)))

    for power, answer in enumerate(moments(values, 0, len(values))):
        commands.append(f"ALL {power}")
        outputs.append(str(answer))
    for power in range(MAX_POWER + 1):
        commands.append(f"QUERY 0 0 {power}")
        outputs.append("0")
    return initial, commands, outputs


def boundary_case() -> tuple[list[int], list[str], list[str]]:
    initial = [0, 1, MASK, 1 << 31, MASK - 1]
    values = initial[:]
    commands: list[str] = []
    outputs: list[str] = []

    def query(left: int, right: int, power: int) -> None:
        commands.append(f"QUERY {left} {right} {power}")
        outputs.append(str(moments(values, left, right)[power]))

    for power in range(MAX_POWER + 1):
        query(0, len(values), power)
    commands.append(f"REVERSE 0 {len(values)}")
    values.reverse()
    for power in range(MAX_POWER + 1):
        query(0, len(values), power)
    commands.append("SET 0 4294967295")
    values[0] = MASK
    commands.append("INSERT 3 4294967295")
    values.insert(3, MASK)
    commands.append("INSERT 0 0")
    values.insert(0, 0)
    for power in range(MAX_POWER + 1):
        query(1, len(values) - 1, power)
    commands.append("ERASE 3")
    values.pop(3)
    commands.append("SIZE")
    outputs.append(str(len(values)))
    for power in range(MAX_POWER + 1):
        commands.append(f"ALL {power}")
        outputs.append(str(moments(values, 0, len(values))[power]))
    return initial, commands, outputs


def maximum_case() -> tuple[list[int], list[str], list[str]]:
    size = 100000
    query_limit = 100000
    initial = [
        ((index * 2654435761) ^ (index << 17) ^ (index >> 3)) & MASK
        for index in range(size)
    ]
    initial[:5] = [0, 1, MASK, 1 << 31, MASK - 1]
    sequence = deque(initial)
    reversed_order = False
    commands: list[str] = []
    outputs: list[str] = []

    for power, answer in enumerate(moments(initial, 0, len(initial))):
        commands.append(f"QUERY 0 {len(initial)} {power}")
        outputs.append(str(answer))

    commands.append(f"REVERSE 0 {len(sequence)}")
    reversed_order = True
    reversed_initial = initial[::-1]
    for power, answer in enumerate(moments(reversed_initial, 0, len(reversed_initial))):
        commands.append(f"ALL {power}")
        outputs.append(str(answer))

    def logical_value(position: int) -> int:
        if reversed_order:
            return sequence[-1 - position]
        return sequence[position]

    base_command_count = len(commands)
    while len(commands) < query_limit:
        phase = (len(commands) - base_command_count) % 8
        step = len(commands)
        value = ((step * 2246822519) ^ (step << 9)) & MASK
        current_size = len(sequence)

        if phase == 0:
            commands.append(f"SET 0 {value}")
            if reversed_order:
                sequence[-1] = value
            else:
                sequence[0] = value
        elif phase == 1:
            right = min(8, current_size)
            power = step % (MAX_POWER + 1)
            prefix = [logical_value(index) for index in range(right)]
            commands.append(f"QUERY 0 {right} {power}")
            outputs.append(str(moments(prefix, 0, len(prefix))[power]))
        elif phase == 2:
            position = current_size - 1
            commands.append(f"SET {position} {value}")
            if reversed_order:
                sequence[0] = value
            else:
                sequence[-1] = value
        elif phase == 3:
            position = current_size - 1
            power = step % (MAX_POWER + 1)
            commands.append(f"QUERY {position} {current_size} {power}")
            outputs.append(str(logical_value(position)))
        elif phase == 4:
            commands.append(f"INSERT {current_size} {value}")
            if reversed_order:
                sequence.appendleft(value)
            else:
                sequence.append(value)
        elif phase == 5:
            position = len(sequence) - 1
            power = step % (MAX_POWER + 1)
            commands.append(f"QUERY {position} {len(sequence)} {power}")
            outputs.append(str(logical_value(position)))
        elif phase == 6:
            position = current_size - 1
            commands.append(f"ERASE {position}")
            if reversed_order:
                sequence.popleft()
            else:
                sequence.pop()
        else:
            commands.append(f"REVERSE 0 {current_size}")
            reversed_order = not reversed_order

    assert len(commands) == query_limit
    return initial, commands, outputs


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071402)
    write_case(out_dir, "case_00", *small_random_case(rng))
    write_case(out_dir, "case_01", *boundary_case())
    write_case(out_dir, "case_02", *maximum_case())


if __name__ == "__main__":
    main()
