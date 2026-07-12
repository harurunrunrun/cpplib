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

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071305)
    n = 51
    versions = [[-1] * n]
    commands = ["GROUPS 0", "SAME 0 0 50", "SIZE 0 23", "LEADER 0 23"]
    outputs = [str(n), "0", "1", "23"]

    for _ in range(1050):
        kind = rng.randrange(11)
        version = rng.randrange(len(versions))
        parent = versions[version]
        if kind < 4:
            u, v = rng.randrange(n), rng.randrange(n)
            commands.append(f"MERGE {version} {u} {v}")
            next_parent = parent.copy()
            merge(next_parent, u, v)
            versions.append(next_parent)
            outputs.append(str(len(versions) - 1))
        elif kind == 4:
            commands.append(f"FORK {version}")
            versions.append(parent.copy())
            outputs.append(str(len(versions) - 1))
        elif kind < 7:
            u, v = rng.randrange(n), rng.randrange(n)
            commands.append(f"SAME {version} {u} {v}")
            outputs.append(str(int(leader(parent, u) == leader(parent, v))))
        elif kind == 7:
            v = rng.randrange(n)
            commands.append(f"SIZE {version} {v}")
            outputs.append(str(-parent[leader(parent, v)]))
        elif kind == 8:
            commands.append(f"GROUPS {version}")
            outputs.append(str(sum(value < 0 for value in parent)))
        else:
            v = rng.randrange(n)
            commands.append(f"LEADER {version} {v}")
            outputs.append(str(leader(parent, v)))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
