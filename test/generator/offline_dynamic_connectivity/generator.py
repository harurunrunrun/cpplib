#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations
import argparse
import random
from collections import deque
from pathlib import Path

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071301)
    n = 42
    commands: list[str] = []
    outputs: list[str] = []
    edge_count: dict[tuple[int, int], int] = {}
    active: list[tuple[int, int]] = []

    def normalize(u: int, v: int) -> tuple[int, int]:
        return (u, v) if u <= v else (v, u)

    def component(start: int) -> list[bool]:
        graph = [[] for _ in range(n)]
        for (u, v), count in edge_count.items():
            if count and u != v:
                graph[u].append(v)
                graph[v].append(u)
        seen = [False] * n
        seen[start] = True
        queue = deque([start])
        while queue:
            v = queue.popleft()
            for to in graph[v]:
                if not seen[to]:
                    seen[to] = True
                    queue.append(to)
        return seen

    def add(u: int, v: int) -> None:
        edge = normalize(u, v)
        commands.append(f"ADD {u} {v}")
        edge_count[edge] = edge_count.get(edge, 0) + 1
        active.append(edge)

    def erase(index: int) -> None:
        u, v = active[index]
        commands.append(f"ERASE {v} {u}")
        edge_count[(u, v)] -= 1
        active[index] = active[-1]
        active.pop()

    for u, v in [(0, 1), (1, 2), (0, 1), (2, 3), (4, 4)]:
        add(u, v)
    commands.append("SAME 0 3")
    outputs.append("1")
    erase(active.index((0, 1)))
    commands.append("SAME 0 3")
    outputs.append("1")
    erase(active.index((0, 1)))
    commands.append("SAME 0 3")
    outputs.append("0")

    for _ in range(650):
        kind = rng.randrange(12)
        if kind < 4 or not active:
            add(rng.randrange(n), rng.randrange(n))
        elif kind < 7:
            erase(rng.randrange(len(active)))
        elif kind < 10:
            u, v = rng.randrange(n), rng.randrange(n)
            commands.append(f"SAME {u} {v}")
            outputs.append(str(int(component(u)[v])))
        elif kind == 10:
            v = rng.randrange(n)
            commands.append(f"SIZE {v}")
            outputs.append(str(sum(component(v))))
        else:
            commands.append("GROUPS")
            remaining = set(range(n))
            groups = 0
            while remaining:
                groups += 1
                seen = component(next(iter(remaining)))
                remaining = {v for v in remaining if not seen[v]}
            outputs.append(str(groups))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
