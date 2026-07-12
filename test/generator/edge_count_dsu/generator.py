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
    n = 73
    parent = [-1] * n
    edge_count = [0] * n
    groups = n
    commands: list[str] = []
    outputs: list[str] = []

    def leader(v: int) -> int:
        if parent[v] < 0:
            return v
        parent[v] = leader(parent[v])
        return parent[v]

    def add_edge(u: int, v: int) -> bool:
        nonlocal groups
        u = leader(u)
        v = leader(v)
        if u == v:
            edge_count[u] += 1
            return False
        if -parent[u] < -parent[v]:
            u, v = v, u
        parent[u] += parent[v]
        parent[v] = u
        edge_count[u] += edge_count[v] + 1
        groups -= 1
        return True

    for _ in range(1300):
        kind = rng.randrange(9)
        if kind < 3:
            u, v = rng.randrange(n), rng.randrange(n)
            commands.append(f"ADD {u} {v}")
            outputs.append(str(int(add_edge(u, v))))
        elif kind == 3:
            u, v = rng.randrange(n), rng.randrange(n)
            commands.append(f"SAME {u} {v}")
            outputs.append(str(int(leader(u) == leader(v))))
        elif kind in {4, 5, 6, 7}:
            v = rng.randrange(n)
            root = leader(v)
            command = {4: "SIZE", 5: "EDGES", 6: "EXCESS", 7: "TREE"}[kind]
            commands.append(f"{command} {v}")
            size = -parent[root]
            edges = edge_count[root]
            answer = {4: size, 5: edges, 6: edges - size + 1, 7: int(edges == size - 1)}[kind]
            outputs.append(str(answer))
        else:
            if rng.randrange(2):
                v = rng.randrange(n)
                root = leader(v)
                commands.append(f"UNICYCLIC {v}")
                outputs.append(str(int(edge_count[root] == -parent[root])))
            else:
                commands.append("GROUPS")
                outputs.append(str(groups))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
