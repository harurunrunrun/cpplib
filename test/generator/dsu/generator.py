#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    n = 64
    parent = [-1] * n
    groups = n
    commands: list[str] = []
    outputs: list[str] = []

    def leader(v: int) -> int:
        if parent[v] < 0:
            return v
        parent[v] = leader(parent[v])
        return parent[v]

    def merge(u: int, v: int) -> bool:
        nonlocal groups
        u = leader(u)
        v = leader(v)
        if u == v:
            return False
        if -parent[u] < -parent[v]:
            u, v = v, u
        parent[u] += parent[v]
        parent[v] = u
        groups -= 1
        return True

    for u, v in [(0, 1), (2, 3), (1, 2), (0, 3)]:
        commands.append(f"MERGE {u} {v}")
        outputs.append(str(int(merge(u, v))))

    for _ in range(700):
        kind = rng.randrange(10)
        if kind < 4:
            u = rng.randrange(n)
            v = rng.randrange(n)
            commands.append(f"MERGE {u} {v}")
            outputs.append(str(int(merge(u, v))))
        elif kind < 6:
            u = rng.randrange(n)
            v = rng.randrange(n)
            commands.append(f"SAME {u} {v}")
            outputs.append(str(int(leader(u) == leader(v))))
        elif kind < 8:
            v = rng.randrange(n)
            commands.append(f"SIZE {v}")
            outputs.append(str(-parent[leader(v)]))
        elif kind == 8:
            commands.append("GROUPS")
            outputs.append(str(groups))
        else:
            v = rng.randrange(n)
            commands.append(f"LEADER {v}")
            outputs.append(str(leader(v)))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
