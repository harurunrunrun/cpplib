#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Case = tuple[list[int], list[list[int]], list[tuple[int, int]]]


def solve(case: Case) -> str:
    weights, bags, edges = case
    tree = [[] for _ in bags]
    for first, second in edges:
        tree[first].append(second)
        tree[second].append(first)
    parent = [-2] * len(bags)
    parent[0] = -1
    order = [0]
    for bag in order:
        for next_bag in tree[bag]:
            if next_bag == parent[bag]:
                continue
            parent[next_bag] = bag
            order.append(next_bag)
    owner = [-1] * len(weights)
    owned = [0] * len(bags)
    for bag, vertices in enumerate(bags):
        for vertex in sorted(vertices):
            if owner[vertex] == -1:
                owner[vertex] = bag
                owned[bag] += weights[vertex]
    subtree = owned[:]
    for bag in reversed(order):
        if parent[bag] >= 0:
            subtree[parent[bag]] += subtree[bag]
    total = subtree[0]
    separator = 0
    while True:
        heavy = -1
        for next_bag in tree[separator]:
            component_weight = (
                subtree[next_bag]
                if parent[next_bag] == separator
                else total - subtree[separator]
            )
            if component_weight > total // 2:
                heavy = next_bag
                break
        if heavy == -1:
            break
        separator = heavy

    separator_vertices = sorted(bags[separator])
    removed = set(separator_vertices)
    component = [-1] * len(bags)
    component_count = 0
    for first in tree[separator]:
        stack = [first]
        component[first] = component_count
        while stack:
            bag = stack.pop()
            for next_bag in tree[bag]:
                if next_bag == separator or component[next_bag] != -1:
                    continue
                component[next_bag] = component_count
                stack.append(next_bag)
        component_count += 1
    component_weights = [0] * component_count
    for vertex, weight in enumerate(weights):
        if vertex not in removed:
            component_weights[component[owner[vertex]]] += weight
    maximum = max(component_weights, default=0)
    values = [separator, len(separator_vertices), *separator_vertices, maximum]
    return " ".join(map(str, values))


def serialize(case: Case) -> list[str]:
    weights, bags, edges = case
    lines = [f"{len(weights)} {len(bags)}", " ".join(map(str, weights))]
    lines.extend(
        " ".join(map(str, [len(bag), *bag]))
        for bag in bags
    )
    lines.extend(f"{first} {second}" for first, second in edges)
    return lines


def path_decomposition(
    random_engine: random.Random, vertex_count: int
) -> Case:
    weights = [random_engine.randrange(0, 50) for _ in range(vertex_count)]
    bags = [
        [vertex, vertex + 1]
        for vertex in range(vertex_count - 1)
    ]
    edges = [(index - 1, index) for index in range(1, len(bags))]
    return weights, bags, edges


def write_case(out_dir: Path, name: str, cases: list[Case]) -> None:
    lines = [str(len(cases))]
    for case in cases:
        lines.extend(serialize(case))
    (out_dir / f"{name}.in").write_text(
        "\n".join(lines) + "\n", encoding="utf-8"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(solve(case) for case in cases) + "\n",
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--seed", type=int, default=20260728)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    random_engine = random.Random(args.seed)
    write_case(
        args.out_dir,
        "fixed",
        [
            ([7], [[0]], []),
            ([1, 1, 1, 1], [[0, 1], [1, 2], [2, 3]], [(0, 1), (1, 2)]),
            ([9, 0, 4], [[0, 1], [1, 2]], [(0, 1)]),
        ],
    )
    write_case(
        args.out_dir,
        "random",
        [
            path_decomposition(random_engine, random_engine.randint(2, 120))
            for _ in range(80)
        ],
    )


if __name__ == "__main__":
    main()
