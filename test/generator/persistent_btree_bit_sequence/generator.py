#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

Entry = tuple[bool, int]


def append_update(
    commands: list[str],
    outputs: list[str],
    versions: list[list[Entry]],
    command: str,
    next_sequence: list[Entry],
    suffix: str = "",
) -> int:
    commands.append(command)
    versions.append(next_sequence)
    version = len(versions) - 1
    outputs.append(str(version) + suffix)
    return version


def query_output(
    sequence: list[Entry],
    command: tuple[object, ...],
) -> str:
    kind = str(command[0])
    if kind == "ACCESS":
        position = int(command[2])
        bit, weight = sequence[position]
        return f"{int(bit)} {weight}"
    if kind == "RANK":
        bit, left, right = bool(command[2]), int(command[3]), int(command[4])
        return str(sum(value_bit == bit for value_bit, _ in sequence[left:right]))
    if kind == "SELECT":
        bit, occurrence = bool(command[2]), int(command[3])
        positions = [
            index
            for index, (value_bit, _) in enumerate(sequence)
            if value_bit == bit
        ]
        return str(
            positions[occurrence]
            if occurrence < len(positions)
            else len(sequence)
        )
    if kind == "STATS":
        left, right = int(command[2]), int(command[3])
        ones_l = sum(bit for bit, _ in sequence[:left])
        ones_r = sum(bit for bit, _ in sequence[:right])
        zero_sum = sum(weight for bit, weight in sequence[left:right] if not bit)
        one_sum = sum(weight for bit, weight in sequence[left:right] if bit)
        return f"{ones_l} {ones_r} {zero_sum} {one_sum}"
    if kind == "FIRST":
        bit, count = bool(command[2]), int(command[3])
        selected = [weight for value_bit, weight in sequence if value_bit == bit]
        return str(sum(selected[:count]))
    if kind == "FIRST_FROM":
        bit, left, count = bool(command[2]), int(command[3]), int(command[4])
        selected = [
            weight
            for value_bit, weight in sequence[left:]
            if value_bit == bit
        ]
        return str(sum(selected[:count]))
    if kind == "SIZE":
        return str(len(sequence))
    raise ValueError(kind)


def write_case(
    out_dir: Path,
    name: str,
    initial: list[Entry],
    commands: list[str],
    outputs: list[str],
) -> None:
    data = [f"{len(initial)} {len(commands)}"]
    data.extend(f"{int(bit)} {weight}" for bit, weight in initial)
    data.extend(commands)
    (out_dir / f"{name}.in").write_text(
        "\n".join(data) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(outputs) + ("\n" if outputs else ""),
        encoding="utf-8",
    )


def make_boundary_cases(out_dir: Path) -> None:
    for length in (0, 1, 63, 64, 127, 128, 129):
        initial = [
            (index % 3 == 0, index * 17 - 91)
            for index in range(length)
        ]
        commands: list[str] = []
        outputs: list[str] = []

        def query(parts: tuple[object, ...]) -> None:
            commands.append(" ".join(map(str, parts)))
            outputs.append(query_output(initial, parts))

        query(("SIZE", 0))
        query(("RANK", 0, 1, 0, length))
        query(("RANK", 0, 0, 0, length))
        query(("STATS", 0, 0, length))
        query(("SELECT", 0, 0, max(0, sum(not bit for bit, _ in initial))))
        query(("SELECT", 0, 1, max(0, sum(bit for bit, _ in initial))))
        query(("FIRST", 0, 0, sum(not bit for bit, _ in initial)))
        query(("FIRST", 0, 1, sum(bit for bit, _ in initial)))
        if length:
            query(("ACCESS", 0, length // 2))
            left = length // 3
            count = sum(bit for bit, _ in initial[left:]) // 2
            query(("FIRST_FROM", 0, 1, left, count))
        write_case(out_dir, f"boundary_{length:03d}", initial, commands, outputs)


def make_branching_case(out_dir: Path) -> None:
    rng = random.Random(2026072601)
    initial = [
        (bool(rng.randrange(2)), rng.randrange(-10_000, 10_001))
        for _ in range(257)
    ]
    versions = [initial]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(1800):
        version = rng.randrange(len(versions))
        sequence = versions[version]
        operation = rng.randrange(14)
        if operation < 4 and len(sequence) < 420:
            position = rng.randrange(len(sequence) + 1)
            bit = bool(rng.randrange(2))
            weight = rng.randrange(-20_000, 20_001)
            next_sequence = sequence.copy()
            next_sequence.insert(position, (bit, weight))
            append_update(
                commands,
                outputs,
                versions,
                f"INSERT {version} {position} {int(bit)} {weight}",
                next_sequence,
            )
        elif operation < 6 and sequence:
            position = rng.randrange(len(sequence))
            bit, weight = sequence[position]
            next_sequence = sequence.copy()
            next_sequence.pop(position)
            append_update(
                commands,
                outputs,
                versions,
                f"ERASE {version} {position}",
                next_sequence,
                f" {int(bit)} {weight}",
            )
        elif operation < 8 and sequence:
            position = rng.randrange(len(sequence))
            bit, old_weight = sequence[position]
            weight = rng.randrange(-20_000, 20_001)
            next_sequence = sequence.copy()
            next_sequence[position] = (bit, weight)
            append_update(
                commands,
                outputs,
                versions,
                f"SETP {version} {position} {weight}",
                next_sequence,
                f" {int(bit)} {old_weight}",
            )
        else:
            query_kind = rng.randrange(7)
            if query_kind == 0:
                parts: tuple[object, ...] = ("SIZE", version)
            elif query_kind == 1 and sequence:
                parts = ("ACCESS", version, rng.randrange(len(sequence)))
            elif query_kind == 2:
                left, right = sorted((
                    rng.randrange(len(sequence) + 1),
                    rng.randrange(len(sequence) + 1),
                ))
                bit = bool(rng.randrange(2))
                parts = ("RANK", version, int(bit), left, right)
            elif query_kind == 3:
                bit = bool(rng.randrange(2))
                occurrence = rng.randrange(
                    sum(value_bit == bit for value_bit, _ in sequence) + 4
                )
                parts = ("SELECT", version, int(bit), occurrence)
            elif query_kind == 4:
                left, right = sorted((
                    rng.randrange(len(sequence) + 1),
                    rng.randrange(len(sequence) + 1),
                ))
                parts = ("STATS", version, left, right)
            elif query_kind == 5:
                bit = bool(rng.randrange(2))
                count = rng.randrange(
                    sum(value_bit == bit for value_bit, _ in sequence) + 1
                )
                parts = ("FIRST", version, int(bit), count)
            else:
                bit = bool(rng.randrange(2))
                left = rng.randrange(len(sequence) + 1)
                available = sum(
                    value_bit == bit for value_bit, _ in sequence[left:]
                )
                count = rng.randrange(available + 1)
                parts = ("FIRST_FROM", version, int(bit), left, count)

            commands.append(" ".join(map(str, parts)))
            outputs.append(query_output(sequence, parts))

    write_case(out_dir, "branching", initial, commands, outputs)


def make_split_merge_case(out_dir: Path) -> None:
    versions: list[list[Entry]] = [[]]
    commands: list[str] = []
    outputs: list[str] = []
    current_version = 0

    for step in range(2305):
        sequence = versions[current_version]
        if step % 3 == 0:
            position = 0
        elif step % 3 == 1:
            position = len(sequence)
        else:
            position = len(sequence) // 2
        bit = step % 5 in (0, 2)
        weight = step * 31 - 40_000
        next_sequence = sequence.copy()
        next_sequence.insert(position, (bit, weight))
        current_version = append_update(
            commands,
            outputs,
            versions,
            f"INSERT {current_version} {position} {int(bit)} {weight}",
            next_sequence,
        )
        if step % 193 == 0:
            parts: tuple[object, ...] = (
                "STATS",
                current_version,
                0,
                len(next_sequence),
            )
            commands.append(" ".join(map(str, parts)))
            outputs.append(query_output(next_sequence, parts))

    original_full_version = current_version
    while versions[current_version]:
        sequence = versions[current_version]
        step = len(sequence)
        if step % 3 == 0:
            position = 0
        elif step % 3 == 1:
            position = len(sequence) - 1
        else:
            position = len(sequence) // 2
        bit, weight = sequence[position]
        next_sequence = sequence.copy()
        next_sequence.pop(position)
        current_version = append_update(
            commands,
            outputs,
            versions,
            f"ERASE {current_version} {position}",
            next_sequence,
            f" {int(bit)} {weight}",
        )
        if len(next_sequence) in (2048, 2047, 129, 128, 127, 64, 63, 1, 0):
            parts = ("SIZE", current_version)
            commands.append(" ".join(map(str, parts)))
            outputs.append(query_output(next_sequence, parts))

    full_sequence = versions[original_full_version]
    for position in (0, 127, 128, 2047, 2304):
        parts = ("ACCESS", original_full_version, position)
        commands.append(" ".join(map(str, parts)))
        outputs.append(query_output(full_sequence, parts))

    write_case(out_dir, "split_merge", [], commands, outputs)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    make_boundary_cases(out_dir)
    make_branching_case(out_dir)
    make_split_merge_case(out_dir)


if __name__ == "__main__":
    main()
