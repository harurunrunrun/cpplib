#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071301)
    values = [rng.randrange(-10**6, 10**6 + 1) for _ in range(70)]
    values[:4] = [10**12, -10**12, 0, 7]
    initial = values[:]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(1800):
        kind = rng.randrange(9)
        if kind == 0 and len(values) < 400:
            p = rng.randrange(len(values) + 1)
            value = rng.randrange(-10**9, 10**9 + 1)
            commands.append(f"INSERT {p} {value}")
            values.insert(p, value)
        elif kind == 1 and values:
            p = rng.randrange(len(values))
            commands.append(f"ERASE {p}")
            values.pop(p)
        elif kind == 2 and values:
            p = rng.randrange(len(values))
            value = rng.randrange(-10**9, 10**9 + 1)
            commands.append(f"SET {p} {value}")
            values[p] = value
        elif kind == 3:
            l, r = sorted((rng.randrange(len(values) + 1), rng.randrange(len(values) + 1)))
            commands.append(f"REVERSE {l} {r}")
            values[l:r] = reversed(values[l:r])
        elif kind in {4, 5, 6}:
            l, r = sorted((rng.randrange(len(values) + 1), rng.randrange(len(values) + 1)))
            commands.append(f"PROD {l} {r}")
            outputs.append(str(sum(values[l:r])))
        elif kind == 7 and values:
            p = rng.randrange(len(values))
            commands.append(f"GET {p}")
            outputs.append(str(values[p]))
        elif kind == 8:
            commands.append("SIZE")
            outputs.append(str(len(values)))
        else:
            commands.append("ALL")
            outputs.append(str(sum(values)))

    commands.extend(["SIZE", "ALL"])
    outputs.extend([str(len(values)), str(sum(values))])
    lines = [f"{len(initial)} {len(commands)}", " ".join(map(str, initial)), *commands]
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()