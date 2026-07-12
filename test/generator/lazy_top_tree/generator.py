#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from collections import deque
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071306)
    n = 70
    values = [rng.randrange(-10**6, 10**6 + 1) for _ in range(n)]
    values[:4] = [10**12, -10**12, 0, 7]
    initial_values = values[:]
    initial_edges = [(v, rng.randrange(v)) for v in range(1, n)]
    graph = [set() for _ in range(n)]
    edges: set[tuple[int, int]] = set()
    for u, v in initial_edges:
        graph[u].add(v)
        graph[v].add(u)
        edges.add((min(u, v), max(u, v)))

    def component(start: int) -> list[int]:
        seen = {start}
        queue = deque([start])
        while queue:
            v = queue.popleft()
            for to in graph[v]:
                if to not in seen:
                    seen.add(to)
                    queue.append(to)
        return sorted(seen)

    def path_vertices(start: int, goal: int) -> list[int]:
        parent = [-1] * n
        parent[start] = start
        queue = deque([start])
        while queue:
            v = queue.popleft()
            if v == goal:
                break
            for to in graph[v]:
                if parent[to] == -1:
                    parent[to] = v
                    queue.append(to)
        result = []
        v = goal
        while True:
            result.append(v)
            if v == start:
                return result
            v = parent[v]

    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(1300):
        kind = rng.randrange(9)
        if kind == 0:
            v = rng.randrange(n)
            value = rng.randrange(-10**9, 10**9 + 1)
            commands.append(f"SET {v} {value}")
            values[v] = value
        elif kind == 1:
            v = rng.randrange(n)
            commands.append(f"GET {v}")
            outputs.append(str(values[v]))
        elif kind == 2:
            u, v = rng.randrange(n), rng.randrange(n)
            commands.append(f"PROD {u} {v}")
            outputs.append(str(sum(values[x] for x in path_vertices(u, v))))
        elif kind == 3:
            u, v = rng.randrange(n), rng.randrange(n)
            commands.append(f"CONNECTED {u} {v}")
            outputs.append("1")
        elif kind == 4:
            u, v = rng.sample(range(n), 2)
            commands.append(f"LINK {u} {v}")
            outputs.append("0")
        elif kind == 5:
            while True:
                u, v = rng.sample(range(n), 2)
                edge = (min(u, v), max(u, v))
                if edge not in edges:
                    break
            commands.append(f"CUT {u} {v}")
            outputs.append("0")
        elif kind == 6:
            a, b = rng.choice(sorted(edges))
            graph[a].remove(b)
            graph[b].remove(a)
            edges.remove((a, b))
            commands.append(f"CUT {a} {b}")
            outputs.append("1")
            commands.append(f"CONNECTED {a} {b}")
            outputs.append("0")
            left = component(a)
            left_set = set(left)
            right = [v for v in range(n) if v not in left_set]
            u, v = rng.choice(left), rng.choice(right)
            graph[u].add(v)
            graph[v].add(u)
            edges.add((min(u, v), max(u, v)))
            commands.append(f"LINK {u} {v}")
            outputs.append("1")
        elif kind == 7:
            u, v = rng.randrange(n), rng.randrange(n)
            delta = rng.randrange(-10**6, 10**6 + 1)
            commands.append(f"APPLY {u} {v} {delta}")
            for x in path_vertices(u, v):
                values[x] += delta
        else:
            v = rng.randrange(n)
            commands.append(f"STATS {v} {len(component(v))}")
            outputs.append("1")

    lines = [f"{n} {len(initial_edges)} {len(commands)}", " ".join(map(str, initial_values))]
    lines.extend(f"{u} {v}" for u, v in initial_edges)
    lines.extend(commands)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()