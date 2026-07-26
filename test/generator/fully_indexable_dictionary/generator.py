#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def select(values: list[int], value: int, occurrence: int) -> int:
    positions = [index for index, item in enumerate(values) if item == value]
    return positions[occurrence] if occurrence < len(positions) else len(values)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    n = 321
    fixed = [rng.randrange(2) for _ in range(n)]
    dynamic = fixed[:]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(1400):
        kind = rng.randrange(17)
        if kind == 0:
            index = rng.randrange(n)
            commands.append(f"FGET {index}")
            outputs.append(str(fixed[index]))
        elif kind == 1:
            value = rng.randrange(2)
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            commands.append(f"FRANK {value} {l} {r}")
            outputs.append(str(fixed[l:r].count(value)))
        elif kind == 2:
            value = rng.randrange(2)
            occurrence = rng.randrange(n + 20)
            commands.append(f"FSELECT {value} {occurrence}")
            outputs.append(str(select(fixed, value, occurrence)))
        elif kind == 3:
            index = rng.randrange(len(dynamic))
            commands.append(f"DGET {index}")
            outputs.append(str(dynamic[index]))
        elif kind in {4, 5}:
            value = rng.randrange(2)
            l, r = sorted((
                rng.randrange(len(dynamic) + 1),
                rng.randrange(len(dynamic) + 1),
            ))
            commands.append(f"DRANK {value} {l} {r}")
            outputs.append(str(dynamic[l:r].count(value)))
        elif kind == 6:
            value = rng.randrange(2)
            occurrence = rng.randrange(len(dynamic) + 20)
            commands.append(f"DSELECT {value} {occurrence}")
            outputs.append(str(select(dynamic, value, occurrence)))
        elif kind < 9:
            index = rng.randrange(len(dynamic))
            value = rng.randrange(2)
            commands.append(f"DSET {index} {value}")
            dynamic[index] = value
        elif kind == 9:
            index = rng.randrange(len(dynamic))
            commands.append(f"DFLIP {index}")
            dynamic[index] ^= 1
        elif kind == 10 and len(dynamic) < 500:
            index = rng.randrange(len(dynamic) + 1)
            value = rng.randrange(2)
            commands.append(f"DINSERT {index} {value}")
            dynamic.insert(index, value)
        elif kind == 11 and len(dynamic) > 1:
            index = rng.randrange(len(dynamic))
            commands.append(f"DERASE {index}")
            outputs.append(str(dynamic.pop(index)))
        elif kind == 12 and len(dynamic) < 500:
            value = rng.randrange(2)
            commands.append(f"DPUSH {value}")
            dynamic.append(value)
        elif kind == 13 and len(dynamic) > 1:
            commands.append("DPOP")
            outputs.append(str(dynamic.pop()))
        elif kind == 14:
            commands.append("DCOPY")
            outputs.append(
                f"{len(dynamic)} {sum(dynamic)} {1 - dynamic[0]} "
                f"{dynamic[0]} {1 - dynamic[0]}"
            )
        elif kind == 15:
            commands.append("DMOVE")
            outputs.append(
                f"{len(dynamic)} {sum(dynamic)} {select(dynamic, 0, 0)}"
            )
        elif kind == 16:
            value = rng.randrange(2)
            r = rng.randrange(len(dynamic) + 1)
            commands.append(f"DRANKP {value} {r}")
            outputs.append(str(dynamic[:r].count(value)))
        else:
            index = rng.randrange(len(dynamic))
            commands.append(f"DGET {index}")
            outputs.append(str(dynamic[index]))
    input_text = (
        f"{n} {len(commands)}\n"
        + " ".join(map(str, fixed))
        + "\n"
        + "\n".join(commands)
        + "\n"
    )
    (out_dir / "case_00.in").write_text(input_text, encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

    boundary = [index & 1 for index in range(510)]
    boundary_commands = [
        "DINSERT 0 1",
        "DPUSH 0",
        "DSIZE",
        "DRANKP 1 512",
        "DCOPY",
        "DMOVE",
        "DERASE 511",
        "DPOP",
        "DSIZE",
    ]
    boundary_outputs = [
        "512",
        "256",
        "512 256 0 1 0",
        "512 256 1",
        "0",
        "1",
        "510",
    ]
    boundary_input = (
        f"510 {len(boundary_commands)}\n"
        + " ".join(map(str, boundary))
        + "\n"
        + "\n".join(boundary_commands)
        + "\n"
    )
    (out_dir / "case_01.in").write_text(boundary_input, encoding="utf-8")
    (out_dir / "case_01.out").write_text(
        "\n".join(boundary_outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
