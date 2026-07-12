#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations
import argparse
import random
from pathlib import Path

def leader(parent: list[int], v: int) -> int:
    while parent[v] >= 0:
        v = parent[v]
    return v

def merge(parent: list[int], u: int, v: int) -> None:
    u, v = leader(parent, u), leader(parent, v)
    if u == v:
        return
    if -parent[u] < -parent[v]:
        u, v = v, u
    parent[u] += parent[v]
    parent[v] = u

def groups(parent: list[int]) -> int:
    return sum(value < 0 for value in parent)

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071304)
    n = 45
    versions = [[-1] * n]
    commands = ["SAME 0 0 1", "SIZE 0 0", "GROUPS 0", "LEADER 0 7", "LATEST"]
    outputs = ["0", "1", str(n), "7", "0"]

    for _ in range(1000):
        kind = rng.randrange(10)
        if kind < 4:
            u, v = rng.randrange(n), rng.randrange(n)
            commands.append(f"MERGE {u} {v}")
            next_parent = versions[-1].copy()
            merge(next_parent, u, v)
            versions.append(next_parent)
            outputs.append(str(len(versions) - 1))
        else:
            version = rng.randrange(len(versions))
            parent = versions[version]
            if kind < 7:
                u, v = rng.randrange(n), rng.randrange(n)
                commands.append(f"SAME {version} {u} {v}")
                outputs.append(str(int(leader(parent, u) == leader(parent, v))))
            elif kind == 7:
                v = rng.randrange(n)
                commands.append(f"SIZE {version} {v}")
                outputs.append(str(-parent[leader(parent, v)]))
            elif kind == 8:
                commands.append(f"GROUPS {version}")
                outputs.append(str(groups(parent)))
            else:
                v = rng.randrange(n)
                commands.append(f"LEADER {version} {v}")
                outputs.append(str(leader(parent, v)))
        if rng.randrange(25) == 0:
            commands.append("LATEST")
            outputs.append(str(len(versions) - 1))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
