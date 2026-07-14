#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def evaluate(
    keys: list[int],
    weights: list[int],
    command: tuple[str, int, int, int],
) -> int:
    kind, left, right, upper = command
    if kind == "ALL":
        return sum(weights[left:right])
    if kind == "LESS":
        return sum(
            weight
            for key, weight in zip(keys[left:right], weights[left:right])
            if key < upper
        )
    return sum(
        weight
        for key, weight in zip(keys[left:right], weights[left:right])
        if key <= upper
    )


def write_case(
    out_dir: Path,
    case_id: int,
    keys: list[int],
    weights: list[int],
    commands: list[tuple[str, int, int, int]],
) -> None:
    input_lines = [
        f"{len(keys)} {len(commands)}",
        " ".join(map(str, keys)),
        " ".join(map(str, weights)),
    ]
    input_lines.extend(
        f"{kind} {left} {right} {upper}"
        for kind, left, right, upper in commands
    )
    output_lines = [
        str(evaluate(keys, weights, command))
        for command in commands
    ]
    stem = f"case_{case_id:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def write_maximum_case(out_dir: Path, case_id: int) -> None:
    size = 300_000
    query_count = 300_000
    keys = [7] * size
    weights = [index % 101 - 50 for index in range(size)]
    prefix = [0]
    for weight in weights:
        prefix.append(prefix[-1] + weight)

    stem = f"case_{case_id:02d}"
    with (
        (out_dir / f"{stem}.in").open("w", encoding="utf-8") as input_file,
        (out_dir / f"{stem}.out").open("w", encoding="utf-8") as output_file,
    ):
        input_file.write(f"{size} {query_count}\n")
        input_file.write(" ".join(map(str, keys)) + "\n")
        input_file.write(" ".join(map(str, weights)) + "\n")
        for query in range(query_count):
            left = query * 1009 % (size + 1)
            remaining = size - left
            right = left + (query * 9176 + 13) % (remaining + 1)
            total = prefix[right] - prefix[left]

            if query % 3 == 0:
                kind = "ALL"
                upper = 0
                answer = total
            elif query % 3 == 1:
                kind = "LESS"
                upper = 8 if query % 2 else 7
                answer = total if upper == 8 else 0
            else:
                kind = "LEQ"
                upper = 7 if query % 2 else 6
                answer = total if upper == 7 else 0

            input_file.write(f"{kind} {left} {right} {upper}\n")
            output_file.write(f"{answer}\n")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    case_id = 0
    write_case(
        out_dir,
        case_id,
        [],
        [],
        [
            ("ALL", 0, 0, 0),
            ("LESS", 0, 0, -1),
            ("LEQ", 0, 0, 1),
        ],
    )
    case_id += 1

    keys = [2, -1, 2, 3, 2, 0]
    weights = [5, 7, -2, 11, 13, 17]
    write_case(
        out_dir,
        case_id,
        keys,
        weights,
        [
            ("ALL", 0, 6, 0),
            ("ALL", 1, 5, 0),
            ("LESS", 0, 6, -1),
            ("LEQ", 0, 6, -1),
            ("LESS", 0, 6, 2),
            ("LEQ", 0, 6, 2),
            ("LEQ", 2, 5, 2),
            ("LESS", 3, 3, 100),
        ],
    )
    case_id += 1

    random_source = random.Random(20260714)
    for size in [1, 2, 3, 31, 32, 33, 211]:
        keys = [random_source.randrange(-20, 21) for _ in range(size)]
        weights = [random_source.randrange(-1000, 1001) for _ in range(size)]
        commands: list[tuple[str, int, int, int]] = []
        for query in range(1200):
            left = random_source.randrange(size + 1)
            right = random_source.randrange(size + 1)
            if right < left:
                left, right = right, left
            kind = ["ALL", "LESS", "LEQ"][query % 3]
            upper = random_source.randrange(-22, 23)
            commands.append((kind, left, right, upper))
        write_case(out_dir, case_id, keys, weights, commands)
        case_id += 1

    write_maximum_case(out_dir, case_id)


if __name__ == "__main__":
    main()
