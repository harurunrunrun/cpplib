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

    rng = random.Random(2026071304)
    n = 73
    initial = [rng.randrange(-200, 201) for _ in range(n)]
    versions = [initial[:]]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(600):
        typ = rng.randrange(11)
        if typ < 2:
            base = rng.randrange(len(versions))
            k = rng.randrange(n)
            value = rng.randrange(-250, 251)
            next_values = versions[base][:]
            next_values[k] = value
            versions.append(next_values)
            commands.append(f"SET {base} {k} {value}")
            outputs.append(str(len(versions) - 1))
        elif typ == 2:
            base = rng.randrange(len(versions))
            versions.append(versions[base][:])
            commands.append(f"FORK {base}")
            outputs.append(str(len(versions) - 1))
        elif typ == 3:
            version = rng.randrange(len(versions))
            k = rng.randrange(n)
            commands.append(f"ACCESS {version} {k}")
            outputs.append(str(versions[version][k]))
        elif typ == 4:
            version = rng.randrange(len(versions))
            value = rng.randrange(-260, 261)
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            commands.append(f"RANK {version} {value} {left} {right}")
            outputs.append(str(versions[version][left:right].count(value)))
        elif typ == 5:
            version = rng.randrange(len(versions))
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            lower = rng.randrange(-260, 261)
            upper = rng.randrange(-260, 261)
            if lower > upper:
                lower, upper = upper, lower
            selected = versions[version][left:right]
            commands.append(f"FREQ {version} {left} {right} {lower} {upper}")
            outputs.append(str(sum(lower <= value < upper for value in selected)))
        elif typ == 6:
            version = rng.randrange(len(versions))
            left = rng.randrange(n)
            right = rng.randrange(left + 1, n + 1)
            k = rng.randrange(right - left)
            commands.append(f"KTH {version} {left} {right} {k}")
            outputs.append(str(sorted(versions[version][left:right])[k]))
        elif typ == 7:
            version = rng.randrange(len(versions))
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            upper = rng.randrange(-260, 261)
            candidates = [
                value for value in versions[version][left:right] if value < upper
            ]
            answer = max(candidates) if candidates else None
            commands.append(f"PREV {version} {left} {right} {upper}")
            outputs.append(optional(answer))
        elif typ == 8:
            version = rng.randrange(len(versions))
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            lower = rng.randrange(-260, 261)
            candidates = [
                value for value in versions[version][left:right] if value >= lower
            ]
            answer = min(candidates) if candidates else None
            commands.append(f"NEXT {version} {left} {right} {lower}")
            outputs.append(optional(answer))
        else:
            version = rng.randrange(len(versions))
            value = rng.randrange(-250, 251)
            occurrence = rng.randrange(6)
            positions = [
                k for k, current in enumerate(versions[version]) if current == value
            ]
            answer = positions[occurrence] if occurrence < len(positions) else n
            commands.append(f"SELECT {version} {value} {occurrence}")
            outputs.append(str(answer))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n"
        + " ".join(map(str, initial))
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