#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def optional(value: int | None) -> str:
    return "NONE" if value is None else str(value)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071306)
    n = 67
    xs = [rng.randrange(-120, 121) for _ in range(n)]
    initial_y = [rng.randrange(-120, 121) for _ in range(n)]
    initial_weight = [rng.randrange(-500, 501) for _ in range(n)]
    versions: list[tuple[list[int], list[int]]] = [
        (initial_y[:], initial_weight[:])
    ]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(550):
        typ = rng.randrange(10)
        if typ < 2:
            base = rng.randrange(len(versions))
            k = rng.randrange(n)
            y = rng.randrange(-140, 141)
            weight = rng.randrange(-600, 601)
            ys = versions[base][0][:]
            weights = versions[base][1][:]
            ys[k] = y
            weights[k] = weight
            versions.append((ys, weights))
            commands.append(f"SET {base} {k} {y} {weight}")
            outputs.append(str(len(versions) - 1))
        elif typ == 2:
            base = rng.randrange(len(versions))
            versions.append((versions[base][0][:], versions[base][1][:]))
            commands.append(f"FORK {base}")
            outputs.append(str(len(versions) - 1))
        else:
            version = rng.randrange(len(versions))
            ys, weights = versions[version]
            if typ == 5:
                point = rng.randrange(n)
                xl = xs[point]
                xr = xl + 1
            else:
                xl = rng.randrange(-150, 151)
                xr = rng.randrange(-150, 151)
                if xl > xr:
                    xl, xr = xr, xl
            indices = [k for k, x in enumerate(xs) if xl <= x < xr]
            if typ == 3:
                yl = rng.randrange(-150, 151)
                yr = rng.randrange(-150, 151)
                if yl > yr:
                    yl, yr = yr, yl
                count = sum(yl <= ys[k] < yr for k in indices)
                commands.append(
                    f"COUNT {version} {xl} {xr} {yl} {yr}"
                )
                outputs.append(f"{count} {count}")
            elif typ == 4:
                yl = rng.randrange(-150, 151)
                yr = rng.randrange(-150, 151)
                if yl > yr:
                    yl, yr = yr, yl
                total = sum(weights[k] for k in indices if yl <= ys[k] < yr)
                commands.append(f"SUM {version} {xl} {xr} {yl} {yr}")
                outputs.append(f"{total} {total}")
            elif typ == 5:
                k = rng.randrange(len(indices))
                ordered = sorted(ys[index] for index in indices)
                commands.append(f"KTH {version} {xl} {xr} {k}")
                outputs.append(str(ordered[k]))
            elif typ < 8:
                upper = rng.randrange(-150, 151)
                candidates = [ys[k] for k in indices if ys[k] < upper]
                answer = max(candidates) if candidates else None
                commands.append(f"PREV {version} {xl} {xr} {upper}")
                outputs.append(optional(answer))
            else:
                lower = rng.randrange(-150, 151)
                candidates = [ys[k] for k in indices if ys[k] >= lower]
                answer = min(candidates) if candidates else None
                commands.append(f"NEXT {version} {xl} {xr} {lower}")
                outputs.append(optional(answer))

    initial_lines = [
        f"{xs[k]} {initial_y[k]} {initial_weight[k]}" for k in range(n)
    ]
    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n"
        + "\n".join(initial_lines)
        + "\n"
        + "\n".join(commands)
        + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()