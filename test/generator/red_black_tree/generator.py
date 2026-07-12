#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import bisect
import random
from pathlib import Path


def optional(values: list[int], index: int) -> str:
    return str(values[index]) if 0 <= index < len(values) else "NONE"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071310)
    values: list[int] = []
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(900):
        typ = rng.randrange(13)
        value = rng.randrange(-220, 221)
        if typ < 2:
            index = bisect.bisect_left(values, value)
            inserted = index == len(values) or values[index] != value
            if inserted:
                values.insert(index, value)
            commands.append(f"INSERT {value}")
            outputs.append(str(int(inserted)))
        elif typ < 4:
            index = bisect.bisect_left(values, value)
            erased = index < len(values) and values[index] == value
            if erased:
                values.pop(index)
            commands.append(f"ERASE {value}")
            outputs.append(str(int(erased)))
        elif typ == 4:
            index = bisect.bisect_left(values, value)
            commands.append(f"CONTAINS {value}")
            outputs.append(
                str(int(index < len(values) and values[index] == value))
            )
        elif typ == 5:
            k = rng.randrange(-3, len(values) + 4)
            commands.append(f"KTH {k}")
            outputs.append(optional(values, k))
        elif typ == 6:
            commands.append(f"ORDER {value}")
            outputs.append(str(bisect.bisect_left(values, value)))
        elif typ == 7:
            commands.append(f"ORDERU {value}")
            outputs.append(str(bisect.bisect_right(values, value)))
        elif typ == 8:
            index = bisect.bisect_left(values, value)
            commands.append(f"LOWER {value}")
            outputs.append(optional(values, index))
        elif typ == 9:
            index = bisect.bisect_right(values, value)
            commands.append(f"UPPER {value}")
            outputs.append(optional(values, index))
        elif typ == 10:
            index = bisect.bisect_right(values, value) - 1
            commands.append(f"LEQ {value}")
            outputs.append(optional(values, index))
        elif typ == 11:
            index = bisect.bisect_left(values, value) - 1
            commands.append(f"LESS {value}")
            outputs.append(optional(values, index))
        else:
            if rng.randrange(5) == 0:
                values.clear()
                commands.append("CLEAR")
                outputs.append("0")
            else:
                commands.append("SIZE")
                outputs.append(str(len(values)))

    (out_dir / "case_00.in").write_text(
        f"{len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()