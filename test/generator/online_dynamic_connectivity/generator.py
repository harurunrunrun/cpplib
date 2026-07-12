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
    rng = random.Random(2026071302)
    n = 48
    commands: list[str] = []
    outputs: list[str] = []
    edge_count: dict[tuple[int, int], int] = {}

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

    def groups() -> int:
        unseen = set(range(n))
        result = 0
        while unseen:
            result += 1
            seen = component(next(iter(unseen)))
            unseen = {v for v in unseen if not seen[v]}
        return result

    for command in ["ADD 0 1", "ADD 1 2", "ADD 0 2", "ERASE 1 2", "ERASE 0 2"]:
        type_, su, sv = command.split()
        u, v = int(su), int(sv)
        edge = normalize(u, v)
        old = edge_count.get(edge, 0)
        if type_ == "ADD":
            edge_count[edge] = old + 1
            outputs.append(str(int(old == 0)))
        else:
            outputs.append(str(int(old > 0)))
            if old > 0:
                edge_count[edge] = old - 1
        commands.append(command)

    for _ in range(950):
        kind = rng.randrange(14)
        u, v = rng.randrange(n), rng.randrange(n)
        edge = normalize(u, v)
        if kind < 4:
            commands.append(f"ADD {u} {v}")
            old = edge_count.get(edge, 0)
            edge_count[edge] = old + 1
            outputs.append(str(int(old == 0)))
        elif kind < 7:
            commands.append(f"ERASE {u} {v}")
            old = edge_count.get(edge, 0)
            outputs.append(str(int(old > 0)))
            if old > 0:
                edge_count[edge] = old - 1
        elif kind < 10:
            commands.append(f"SAME {u} {v}")
            outputs.append(str(int(component(u)[v])))
        elif kind == 10:
            commands.append(f"SIZE {u}")
            outputs.append(str(sum(component(u))))
        elif kind == 11:
            commands.append(f"MULT {u} {v}")
            outputs.append(str(edge_count.get(edge, 0)))
        else:
            commands.append("GROUPS")
            outputs.append(str(groups()))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

    n = 160
    commands = []
    outputs = []
    edge_count = {}

    def add(u: int, v: int) -> None:
        edge = normalize(u, v)
        old = edge_count.get(edge, 0)
        edge_count[edge] = old + 1
        commands.append(f"ADD {u} {v}")
        outputs.append(str(int(old == 0)))

    def erase(u: int, v: int) -> None:
        edge = normalize(u, v)
        old = edge_count.get(edge, 0)
        commands.append(f"ERASE {u} {v}")
        outputs.append(str(int(old > 0)))
        if old > 0:
            edge_count[edge] = old - 1

    def query_all(vertex: int) -> None:
        seen = component(vertex)
        commands.append(f"SIZE {vertex}")
        outputs.append(str(sum(seen)))
        commands.append("GROUPS")
        outputs.append(str(groups()))
        commands.append(f"SAME 0 {n - 1}")
        outputs.append(str(int(component(0)[n - 1])))

    for vertex in range(n - 1):
        add(vertex, vertex + 1)
    for vertex in range(n - 2):
        add(vertex, vertex + 2)
    add(0, n - 1)
    add(7, 7)
    add(7, 7)
    commands.append("MULT 7 7")
    outputs.append("2")
    erase(7, 7)
    erase(7, 7)
    for vertex in range(n - 1):
        erase(vertex, vertex + 1)
        if vertex % 19 == 0:
            query_all(vertex)
    query_all(0)
    erase(0, n - 1)
    query_all(0)
    for vertex in range(0, n - 2, 4):
        erase(vertex, vertex + 2)
        if vertex % 20 == 0:
            query_all(vertex)

    (out_dir / "case_01.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_01.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )

if __name__ == "__main__":
    main()
