#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def all_distances(n, edges):
    graph = [[] for _ in range(n)]
    for u, v, weight in edges:
        graph[u].append((v, weight))
        graph[v].append((u, weight))
    result = [[0] * n for _ in range(n)]
    for source in range(n):
        stack = [(source, -1, 0)]
        while stack:
            v, parent, distance = stack.pop()
            result[source][v] = distance
            for to, weight in graph[v]:
                if to != parent:
                    stack.append((to, v, distance + weight))
    return result


def write_case(out_dir, index, n, edges, operations):
    distances = all_distances(n, edges)
    marked = [True] * n
    output = []
    for operation in operations:
        if operation[0] == "C":
            marked[operation[1]] = not marked[operation[1]]
        else:
            vertices = [v for v in range(n) if marked[v]]
            if not vertices:
                output.append("They have disappeared.")
            else:
                output.append(str(max(distances[u][v] for u in vertices for v in vertices)))
    lines = [str(n)] + [f"{u + 1} {v + 1} {w}" for u, v, w in edges]
    lines += [str(len(operations))]
    lines += ["A" if op[0] == "A" else f"C {op[1] + 1}" for op in operations]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(output) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [(1, [], [("A",), ("C", 0), ("A",), ("C", 0), ("A",)])]
    rng = random.Random(730004)
    for n in range(2, 14):
        edges = [(v, rng.randrange(v), rng.randint(1, 20)) for v in range(1, n)]
        operations = []
        for _ in range(45):
            operations.append(("A",) if rng.randrange(3) == 0
                              else ("C", rng.randrange(n)))
        operations.append(("A",))
        cases.append((n, edges, operations))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
