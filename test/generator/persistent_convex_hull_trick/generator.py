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
    rng = random.Random(2026071307)
    versions: list[list[tuple[int, int]]] = [[]]
    last_slope = [500]
    commands = ["EMPTY 0", "ADD 0 10 7"]
    outputs = ["1", "1"]
    versions.append([(10, 7)])
    last_slope.append(10)

    for _ in range(950):
        kind = rng.randrange(10)
        version = rng.randrange(len(versions))
        lines = versions[version]
        if kind < 5:
            slope = last_slope[version] - rng.randrange(4)
            intercept = rng.randrange(-10**9, 10**9)
            commands.append(f"ADD {version} {slope} {intercept}")
            versions.append(lines + [(slope, intercept)])
            last_slope.append(slope)
            outputs.append(str(len(versions) - 1))
        elif kind == 5:
            commands.append(f"FORK {version}")
            versions.append(lines.copy())
            last_slope.append(last_slope[version])
            outputs.append(str(len(versions) - 1))
        elif kind < 9 and lines:
            x = rng.randrange(-10**6, 10**6)
            commands.append(f"QUERY {version} {x}")
            outputs.append(str(min(a * x + b for a, b in lines)))
        else:
            commands.append(f"EMPTY {version}")
            outputs.append(str(int(not lines)))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
