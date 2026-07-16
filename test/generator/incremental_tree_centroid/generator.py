#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def make_case(seed: int, size: int) -> tuple[list[str], list[str]]:
    rng = random.Random(seed)
    parent = [-1]
    depth = [0]
    subtree = [1]
    centroid = 0
    commands = ["R"]
    output = ["0 0"]
    for vertex in range(1, size):
        selected_parent = rng.randrange(vertex)
        parent.append(selected_parent)
        depth.append(depth[selected_parent] + 1)
        subtree.append(1)
        current = selected_parent
        while current != -1:
            subtree[current] += 1
            current = parent[current]

        current = vertex
        while depth[current] > depth[centroid] + 1:
            current = parent[current]
        if depth[current] == depth[centroid] + 1 and parent[current] == centroid:
            neighbor = current
            component = subtree[neighbor]
        else:
            neighbor = parent[centroid]
            component = vertex + 1 - subtree[centroid]
        if component * 2 > vertex + 1:
            centroid = neighbor
        commands.append(f"A {selected_parent}")
        output.append(f"{vertex} {centroid}")
        if vertex % 131 == 0:
            probe = rng.randrange(vertex + 1)
            commands.append(f"Q {probe}")
            output.append(str(subtree[probe]))
    return commands, output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    directory = Path(args.out_dir)
    directory.mkdir(parents=True, exist_ok=True)
    for index, (seed, size) in enumerate([
        (2026071632, 1), (2026071633, 1000), (2026071634, 50000)
    ]):
        commands, output = make_case(seed, size)
        (directory / f"case_{index:02d}.in").write_text(
            f"{len(commands)}\n" + "\n".join(commands) + "\n", encoding="utf-8"
        )
        (directory / f"case_{index:02d}.out").write_text(
            "\n".join(output) + "\n", encoding="utf-8"
        )


if __name__ == "__main__":
    main()
