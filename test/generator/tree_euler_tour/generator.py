#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def build_reference(
    n: int,
    root: int,
    edges: list[tuple[int, int]],
) -> tuple[list[int], list[int], list[int], list[int], list[int], list[tuple[int, int]]]:
    graph = [[] for _ in range(n)]
    for u, v in edges:
        graph[u].append(v)
        graph[v].append(u)

    parent = [-2] * n
    depth = [0] * n
    tin = [-1] * n
    tout = [-1] * n
    preorder: list[int] = []
    events: list[tuple[int, int]] = []

    def dfs(vertex: int, p: int) -> None:
        parent[vertex] = p
        tin[vertex] = len(preorder)
        preorder.append(vertex)
        events.append((vertex, 1))
        for to in graph[vertex]:
            if to != p:
                dfs(to, vertex)
        tout[vertex] = len(preorder)
        events.append((vertex, 0))

    dfs(root, -1)
    return parent, depth_from_parent(parent, root), tin, tout, preorder, events


def depth_from_parent(parent: list[int], root: int) -> list[int]:
    children = [[] for _ in parent]
    for vertex, p in enumerate(parent):
        if p != -1:
            children[p].append(vertex)
    depth = [0] * len(parent)
    stack = [root]
    while stack:
        vertex = stack.pop()
        for child in children[vertex]:
            depth[child] = depth[vertex] + 1
            stack.append(child)
    return depth


def solve(
    n: int,
    root: int,
    edges: list[tuple[int, int]],
    commands: list[str],
) -> str:
    parent, depth, tin, tout, preorder, events = build_reference(
        n,
        root,
        edges,
    )
    output: list[str] = []

    for command in commands:
        fields = command.split()
        kind = fields[0]
        if kind == "PARENT":
            output.append(str(parent[int(fields[1])]))
        elif kind == "DEPTH":
            output.append(str(depth[int(fields[1])]))
        elif kind == "TIN":
            output.append(str(tin[int(fields[1])]))
        elif kind == "TOUT":
            output.append(str(tout[int(fields[1])]))
        elif kind == "RANGE":
            vertex = int(fields[1])
            output.append(f"{tin[vertex]} {tout[vertex]}")
        elif kind == "SUBSIZE":
            vertex = int(fields[1])
            output.append(str(tout[vertex] - tin[vertex]))
        elif kind == "ANCESTOR":
            ancestor, descendant = map(int, fields[1:3])
            output.append(str(int(
                tin[ancestor] <= tin[descendant] < tout[ancestor]
            )))
        elif kind == "PREORDER_VERTEX":
            output.append(str(preorder[int(fields[1])]))
        elif kind == "PREORDER":
            output.append(
                " ".join([str(len(preorder)), *map(str, preorder)])
            )
        elif kind == "EULER_SIZE":
            output.append(str(len(events)))
        elif kind == "EVENT":
            vertex, entering = events[int(fields[1])]
            output.append(f"{vertex} {entering}")
        elif kind == "EULER":
            values = [str(len(events))]
            for vertex, entering in events:
                values.extend((str(vertex), str(entering)))
            output.append(" ".join(values))
        elif kind == "ROOT":
            output.append(str(root))
        elif kind == "SIZE":
            output.append(str(n))
        elif kind == "EDGES":
            output.append(str(len(edges)))
        else:
            raise AssertionError(kind)

    return "\n".join(output) + ("\n" if output else "")


def write_case(
    out_dir: Path,
    index: int,
    n: int,
    root: int,
    edges: list[tuple[int, int]],
    commands: list[str],
) -> None:
    stem = f"case_{index:02d}"
    lines = [f"{n} {root}"]
    lines.extend(f"{u} {v}" for u, v in edges)
    lines.append(str(len(commands)))
    lines.extend(commands)
    (out_dir / f"{stem}.in").write_text(
        "\n".join(lines) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{stem}.out").write_text(
        solve(n, root, edges, commands),
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed_edges = [(0, 1), (0, 2), (1, 3), (1, 4), (2, 5)]
    fixed_commands = [
        "SIZE",
        "EDGES",
        "ROOT",
        "PREORDER",
        "EULER_SIZE",
        "EULER",
        "RANGE 1",
        "SUBSIZE 2",
        "ANCESTOR 0 5",
        "ANCESTOR 1 5",
    ]
    for vertex in range(6):
        fixed_commands.extend([
            f"PARENT {vertex}",
            f"DEPTH {vertex}",
            f"TIN {vertex}",
            f"TOUT {vertex}",
            f"PREORDER_VERTEX {vertex}",
        ])
    for index in range(12):
        fixed_commands.append(f"EVENT {index}")
    write_case(out_dir, 0, 6, 0, fixed_edges, fixed_commands)

    rng = random.Random(20261018)
    for case_index, n in enumerate((1, 37, 100), 1):
        edges = [
            (rng.randrange(vertex), vertex)
            for vertex in range(1, n)
        ]
        rng.shuffle(edges)
        root = rng.randrange(n)
        commands = ["SIZE", "EDGES", "ROOT", "PREORDER", "EULER"]
        for vertex in range(n):
            commands.extend([
                f"PARENT {vertex}",
                f"DEPTH {vertex}",
                f"RANGE {vertex}",
                f"SUBSIZE {vertex}",
                f"PREORDER_VERTEX {vertex}",
            ])
        for _ in range(300):
            ancestor = rng.randrange(n)
            descendant = rng.randrange(n)
            commands.append(f"ANCESTOR {ancestor} {descendant}")
            commands.append(f"EVENT {rng.randrange(2 * n)}")
            vertex = rng.randrange(n)
            commands.append(rng.choice([
                f"TIN {vertex}",
                f"TOUT {vertex}",
                f"DEPTH {vertex}",
            ]))
        write_case(out_dir, case_index, n, root, edges, commands)


if __name__ == "__main__":
    main()
