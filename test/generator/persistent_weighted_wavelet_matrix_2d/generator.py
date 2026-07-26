#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def rectangle_count(
    xs: list[int],
    ys: list[int],
    xl: int,
    xr: int,
    yl: int,
    yr: int,
) -> int:
    return sum(
        xl <= xs[index] < xr and yl <= ys[index] < yr
        for index in range(len(xs))
    )


def rectangle_sum(
    xs: list[int],
    ys: list[int],
    weights: list[int],
    xl: int,
    xr: int,
    yl: int,
    yr: int,
) -> int:
    return sum(
        weights[index]
        for index in range(len(xs))
        if xl <= xs[index] < xr and yl <= ys[index] < yr
    )


def rectangle(rng: random.Random) -> tuple[int, int, int, int]:
    xl, xr = sorted((rng.randrange(-28, 29), rng.randrange(-28, 29)))
    yl, yr = sorted((rng.randrange(-45, 46), rng.randrange(-45, 46)))
    return xl, xr, yl, yr


def build_full(seed: int, n: int, query_count: int) -> tuple[str, str]:
    rng = random.Random(seed)
    xs = [rng.randrange(-20, 21) for _ in range(n)]
    initial_y = [rng.randrange(-35, 36) for _ in range(n)]
    initial_weight = [rng.randrange(-300, 301) for _ in range(n)]
    versions: list[tuple[list[int], list[int]]] = [
        (initial_y[:], initial_weight[:])
    ]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(query_count):
        operation = rng.randrange(10)
        if operation == 0:
            base = rng.randrange(len(versions))
            position = rng.randrange(n)
            y = rng.randrange(-40, 41)
            weight = rng.randrange(-400, 401)
            ys = versions[base][0][:]
            weights = versions[base][1][:]
            ys[position] = y
            weights[position] = weight
            versions.append((ys, weights))
            commands.append(f"SET {base} {position} {y} {weight}")
            outputs.append(str(len(versions) - 1))
        elif operation == 1:
            base = rng.randrange(len(versions))
            position = rng.randrange(n)
            y = rng.randrange(-40, 41)
            ys = versions[base][0][:]
            weights = versions[base][1][:]
            ys[position] = y
            versions.append((ys, weights))
            commands.append(f"SET_Y {base} {position} {y}")
            outputs.append(str(len(versions) - 1))
        elif operation == 2:
            base = rng.randrange(len(versions))
            position = rng.randrange(n)
            weight = rng.randrange(-400, 401)
            ys = versions[base][0][:]
            weights = versions[base][1][:]
            weights[position] = weight
            versions.append((ys, weights))
            commands.append(f"SET_WEIGHT {base} {position} {weight}")
            outputs.append(str(len(versions) - 1))
        elif operation == 3:
            base = rng.randrange(len(versions))
            versions.append((versions[base][0][:], versions[base][1][:]))
            commands.append(f"FORK {base}")
            outputs.append(str(len(versions) - 1))
        elif operation == 4:
            version = rng.randrange(len(versions))
            position = rng.randrange(n)
            commands.append(f"POINT {version} {position}")
            outputs.append(
                f"{xs[position]} {versions[version][0][position]} "
                f"{versions[version][1][position]}"
            )
        elif operation in (5, 6, 7, 8):
            version = rng.randrange(len(versions))
            xl, xr, yl, yr = rectangle(rng)
            ys, weights = versions[version]
            if operation in (5, 6):
                commands.append(
                    f"COUNT {version} {xl} {xr} {yl} {yr}"
                )
                outputs.append(
                    str(rectangle_count(xs, ys, xl, xr, yl, yr))
                )
            else:
                commands.append(
                    f"SUM {version} {xl} {xr} {yl} {yr}"
                )
                outputs.append(
                    str(rectangle_sum(
                        xs, ys, weights, xl, xr, yl, yr
                    ))
                )
        else:
            commands.append("VERSIONS")
            outputs.append(f"{len(versions)} {len(versions) - 1}")

    text = (
        f"FULL {n} {len(commands)}\n"
        + " ".join(map(str, xs))
        + "\n"
        + " ".join(map(str, initial_y))
        + "\n"
        + " ".join(map(str, initial_weight))
        + "\n"
        + "\n".join(commands)
        + "\n"
    )
    return text, "\n".join(outputs) + "\n"


def build_partial(seed: int, n: int, query_count: int) -> tuple[str, str]:
    rng = random.Random(seed)
    xs = [rng.randrange(-20, 21) for _ in range(n)]
    initial_y = [rng.randrange(-35, 36) for _ in range(n)]
    initial_weight = [rng.randrange(-300, 301) for _ in range(n)]
    versions: list[tuple[list[int], list[int]]] = [
        (initial_y[:], initial_weight[:])
    ]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(query_count):
        operation = rng.randrange(9)
        if operation == 0:
            position = rng.randrange(n)
            y = rng.randrange(-40, 41)
            weight = rng.randrange(-400, 401)
            ys = versions[-1][0][:]
            weights = versions[-1][1][:]
            ys[position] = y
            weights[position] = weight
            versions.append((ys, weights))
            commands.append(f"SET {position} {y} {weight}")
            outputs.append(str(len(versions) - 1))
        elif operation == 1:
            position = rng.randrange(n)
            y = rng.randrange(-40, 41)
            ys = versions[-1][0][:]
            weights = versions[-1][1][:]
            ys[position] = y
            versions.append((ys, weights))
            commands.append(f"SET_Y {position} {y}")
            outputs.append(str(len(versions) - 1))
        elif operation == 2:
            position = rng.randrange(n)
            weight = rng.randrange(-400, 401)
            ys = versions[-1][0][:]
            weights = versions[-1][1][:]
            weights[position] = weight
            versions.append((ys, weights))
            commands.append(f"SET_WEIGHT {position} {weight}")
            outputs.append(str(len(versions) - 1))
        elif operation == 3:
            version = rng.randrange(len(versions))
            position = rng.randrange(n)
            commands.append(f"POINT {version} {position}")
            outputs.append(
                f"{xs[position]} {versions[version][0][position]} "
                f"{versions[version][1][position]}"
            )
        elif operation in (4, 5, 6, 7):
            version = rng.randrange(len(versions))
            xl, xr, yl, yr = rectangle(rng)
            ys, weights = versions[version]
            if operation in (4, 5):
                commands.append(
                    f"COUNT {version} {xl} {xr} {yl} {yr}"
                )
                outputs.append(
                    str(rectangle_count(xs, ys, xl, xr, yl, yr))
                )
            else:
                commands.append(
                    f"SUM {version} {xl} {xr} {yl} {yr}"
                )
                outputs.append(
                    str(rectangle_sum(
                        xs, ys, weights, xl, xr, yl, yr
                    ))
                )
        else:
            commands.append("VERSIONS")
            outputs.append(f"{len(versions)} {len(versions) - 1}")

    text = (
        f"PARTIAL {n} {len(commands)}\n"
        + " ".join(map(str, xs))
        + "\n"
        + " ".join(map(str, initial_y))
        + "\n"
        + " ".join(map(str, initial_weight))
        + "\n"
        + "\n".join(commands)
        + "\n"
    )
    return text, "\n".join(outputs) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        build_full(2026072601, 67, 480),
        build_full(2026072602, 1, 260),
        build_partial(2026072603, 73, 480),
        build_partial(2026072604, 1, 260),
    ]
    for index, (input_text, output_text) in enumerate(cases):
        (out_dir / f"case_{index:02d}.in").write_text(
            input_text, encoding="utf-8"
        )
        (out_dir / f"case_{index:02d}.out").write_text(
            output_text, encoding="utf-8"
        )


if __name__ == "__main__":
    main()
