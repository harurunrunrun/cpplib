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
    rng = random.Random(2026071303)
    n = 37
    initial = [rng.randrange(-10**12, 10**12) for _ in range(n)]
    versions = [initial.copy()]
    commands = ["GET 0 0", "CURRENT 36", "VERSIONS", "LATEST"]
    outputs = [str(initial[0]), str(initial[36]), "1", "0"]

    for _ in range(850):
        kind = rng.randrange(10)
        if kind < 4:
            k = rng.randrange(n)
            value = rng.randrange(-10**15, 10**15)
            commands.append(f"SET {k} {value}")
            next_values = versions[-1].copy()
            next_values[k] = value
            versions.append(next_values)
            outputs.append(str(len(versions) - 1))
        elif kind < 7:
            version = rng.randrange(len(versions))
            k = rng.randrange(n)
            commands.append(f"GET {version} {k}")
            outputs.append(str(versions[version][k]))
        elif kind == 7:
            k = rng.randrange(n)
            commands.append(f"CURRENT {k}")
            outputs.append(str(versions[-1][k]))
        elif kind == 8:
            commands.append("VERSIONS")
            outputs.append(str(len(versions)))
        else:
            commands.append("LATEST")
            outputs.append(str(len(versions) - 1))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + " ".join(map(str, initial)) + "\n"
        + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
