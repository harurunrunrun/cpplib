#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def connected(size, edges, source, target):
    graph = [[] for _ in range(size)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    seen = {source}
    stack = [source]
    while stack:
        vertex = stack.pop()
        for next_vertex in graph[vertex]:
            if next_vertex not in seen:
                seen.add(next_vertex)
                stack.append(next_vertex)
    return target in seen


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76015)
    size = 17
    all_edges = [(left, right) for left in range(size) for right in range(left + 1, size)]
    edges = set()
    commands = []
    answers = []
    for step in range(220):
        action = step % 5
        missing = [edge for edge in all_edges if edge not in edges]
        if action in (0, 1) and missing:
            edge = rng.choice(missing)
            edges.add(edge)
            commands.append(f"add {edge[0] + 1} {edge[1] + 1}")
        elif action == 2 and edges:
            edge = rng.choice(sorted(edges))
            edges.remove(edge)
            commands.append(f"rem {edge[0] + 1} {edge[1] + 1}")
        else:
            left = rng.randrange(size)
            right = rng.randrange(size)
            commands.append(f"conn {left + 1} {right + 1}")
            answers.append("YES" if connected(size, edges, left, right) else "NO")
    input_lines = [f"{size} {len(commands)}"] + commands
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(answers) + "\n")
    (out_dir / "case_01.in").write_text(
        "1 4\nconn 1 1\nconn 1 1\nconn 1 1\nconn 1 1\n"
    )
    (out_dir / "case_01.out").write_text("YES\nYES\nYES\nYES\n")



if __name__ == "__main__":
    main()
