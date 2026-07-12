#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations
import argparse
import random
from pathlib import Path

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071308)
    versions: list[list[tuple[int, int]]] = [[]]
    commands = ["EMPTY 0", "QUERY 0 -1000", "ADD 0 5 11", "ADD 1 -7 23"]
    outputs = ["1", "EMPTY", "1", "2"]
    versions.append([(5, 11)])
    versions.append([(5, 11), (-7, 23)])

    for _ in range(1000):
        kind = rng.randrange(10)
        version = rng.randrange(len(versions))
        lines = versions[version]
        if kind < 5:
            a = rng.randrange(-10**9, 10**9)
            b = rng.randrange(-10**12, 10**12)
            commands.append(f"ADD {version} {a} {b}")
            versions.append(lines + [(a, b)])
            outputs.append(str(len(versions) - 1))
        elif kind == 5:
            commands.append(f"FORK {version}")
            versions.append(lines.copy())
            outputs.append(str(len(versions) - 1))
        elif kind < 9:
            x = rng.randrange(-1000, 1001)
            commands.append(f"QUERY {version} {x}")
            answer = min((a * x + b for a, b in lines), default=None)
            outputs.append("EMPTY" if answer is None else str(answer))
        else:
            commands.append(f"EMPTY {version}")
            outputs.append(str(int(not lines)))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

    fixed_commands = [
        "EMPTY 0",
        "ADD 0 0 3000000000000000000",
        "QUERY 1 0",
        "ADD 0 9000000000000000000 1000000000000000000",
        "QUERY 2 1000",
        "ADD 0 -9000000000000000000 -1000000000000000000",
        "QUERY 3 1000",
    ]
    fixed_outputs = [
        "1", "1", "3000000000000000000",
        "2", str((1 << 63) - 1),
        "3", str(-(1 << 63)),
    ]
    (out_dir / "case_01.in").write_text(
        str(len(fixed_commands)) + "\n" + "\n".join(fixed_commands) + "\n",
        encoding="utf-8")
    (out_dir / "case_01.out").write_text(
        "\n".join(fixed_outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
