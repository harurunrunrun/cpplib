#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def rectangle_sum(
    values: list[list[int]],
    top: int,
    left: int,
    bottom: int,
    right: int,
) -> int:
    return sum(
        values[row][col]
        for row in range(top, bottom)
        for col in range(left, right)
    )


def make_case(
    out_dir: Path,
    index: int,
    height: int,
    width: int,
    query_count: int,
    seed: int,
) -> None:
    rng = random.Random(seed)
    initial = [
        [rng.randint(-30, 30) for _ in range(width)]
        for _ in range(height)
    ]
    versions = [[row[:] for row in initial]]
    commands: list[str] = []
    output: list[str] = []

    for _ in range(query_count):
        choice = rng.randrange(10)
        if choice < 4:
            source = rng.randrange(len(versions))
            row = rng.randrange(height)
            col = rng.randrange(width)
            value = rng.randint(-50, 50)
            next_values = [line[:] for line in versions[source]]
            if choice < 3:
                commands.append(f"SET {source} {row} {col} {value}")
                next_values[row][col] = value
            else:
                commands.append(f"ADD {source} {row} {col} {value}")
                next_values[row][col] += value
            versions.append(next_values)
            output.append(str(len(versions) - 1))
        elif choice == 4:
            source = rng.randrange(len(versions))
            commands.append(f"FORK {source}")
            versions.append([row[:] for row in versions[source]])
            output.append(str(len(versions) - 1))
        elif choice < 7:
            version = rng.randrange(len(versions))
            row = rng.randrange(height)
            col = rng.randrange(width)
            commands.append(f"GET {version} {row} {col}")
            output.append(str(versions[version][row][col]))
        elif choice < 9:
            version = rng.randrange(len(versions))
            top, bottom = sorted((rng.randrange(height + 1), rng.randrange(height + 1)))
            left, right = sorted((rng.randrange(width + 1), rng.randrange(width + 1)))
            commands.append(f"PROD {version} {top} {left} {bottom} {right}")
            output.append(str(rectangle_sum(versions[version], top, left, bottom, right)))
        else:
            if rng.randrange(2):
                version = rng.randrange(len(versions))
                commands.append(f"ALL {version}")
                output.append(str(rectangle_sum(versions[version], 0, 0, height, width)))
            else:
                commands.append("VERSIONS")
                output.append(f"{len(versions)} {len(versions) - 1}")

    input_lines = [f"{height} {width} {len(commands)}"]
    input_lines.extend(" ".join(map(str, row)) for row in initial)
    input_lines.extend(commands)
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{stem}.out").write_text(
        "\n".join(output) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, parameters in enumerate((
        (1, 1, 80, 2026071301),
        (4, 5, 260, 2026071302),
        (8, 8, 420, 2026071303),
    )):
        make_case(out_dir, index, *parameters)


if __name__ == "__main__":
    main()
