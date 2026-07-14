#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def disconnected_pairs(n, edges, active):
    graph = [[] for _ in range(n)]
    for index, (left, right) in enumerate(edges):
        if active[index]:
            graph[left].append(right)
            graph[right].append(left)
    seen = [False] * n
    connected_pairs = 0
    for start in range(n):
        if seen[start]:
            continue
        stack = [start]
        seen[start] = True
        size = 0
        while stack:
            vertex = stack.pop()
            size += 1
            for neighbor in graph[vertex]:
                if not seen[neighbor]:
                    seen[neighbor] = True
                    stack.append(neighbor)
        connected_pairs += size * (size - 1) // 2
    return n * (n - 1) // 2 - connected_pairs


def make_case(rng, n):
    edges = [(rng.randrange(vertex), vertex) for vertex in range(1, n)]
    active = [True] * len(edges)
    order = list(range(len(edges)))
    rng.shuffle(order)
    queries = ["Q"]
    answers = [str(disconnected_pairs(n, edges, active))]
    for edge in order:
        queries.append(f"R {edge + 1}")
        active[edge] = False
        queries.append("Q")
        answers.append(str(disconnected_pairs(n, edges, active)))
        if rng.randrange(2):
            queries.append("Q")
            answers.append(answers[-1])
    return edges, queries, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(7199)
    cases = [make_case(rng, n) for n in (1, 2, 8, 20)]
    lines = [str(len(cases))]
    expected = []
    for edges, queries, answers in cases:
        lines.append(str(len(edges) + 1))
        lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
        lines.append(str(len(queries)))
        lines.extend(queries)
        expected.extend(answers)
        expected.append("")
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(expected) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
