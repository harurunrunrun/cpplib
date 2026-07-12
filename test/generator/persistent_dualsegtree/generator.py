#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations
import argparse
import random
from pathlib import Path

MOD = 998244353

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071309)
    n = 53
    initial = [rng.randrange(MOD) for _ in range(n)]
    versions = [initial.copy()]
    commands = ["GET 0 0", f"GET 0 {n - 1}"]
    outputs = [str(initial[0]), str(initial[-1])]

    for _ in range(1050):
        kind = rng.randrange(10)
        version = rng.randrange(len(versions))
        values = versions[version]
        if kind < 5:
            l, r = rng.randrange(n + 1), rng.randrange(n + 1)
            if l > r:
                l, r = r, l
            a, b = rng.randrange(MOD), rng.randrange(MOD)
            commands.append(f"APPLY {version} {l} {r} {a} {b}")
            next_values = values.copy()
            for k in range(l, r):
                next_values[k] = (a * next_values[k] + b) % MOD
            versions.append(next_values)
            outputs.append(str(len(versions) - 1))
        elif kind < 7:
            k, value = rng.randrange(n), rng.randrange(MOD)
            commands.append(f"SET {version} {k} {value}")
            next_values = values.copy()
            next_values[k] = value
            versions.append(next_values)
            outputs.append(str(len(versions) - 1))
        elif kind == 7:
            commands.append(f"FORK {version}")
            versions.append(values.copy())
            outputs.append(str(len(versions) - 1))
        else:
            k = rng.randrange(n)
            commands.append(f"GET {version} {k}")
            outputs.append(str(values[k]))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + " ".join(map(str, initial)) + "\n"
        + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
