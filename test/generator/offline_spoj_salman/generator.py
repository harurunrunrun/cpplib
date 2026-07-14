#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def descendants(parents):
    n = len(parents)
    children = [[] for _ in range(n)]
    for vertex, parent in enumerate(parents):
        if parent != -1:
            children[parent].append(vertex)
    result = []
    for root in range(n):
        vertices = []
        stack = [root]
        while stack:
            v = stack.pop()
            vertices.append(v)
            stack.extend(children[v])
        result.append(vertices)
    return result


def write_case(out_dir, index, groups):
    lines = [str(len(groups))]
    output = []
    for test, (parents, initial, operations) in enumerate(groups, 1):
        salary = initial[:]
        subtree = descendants(parents)
        lines.append(f"{len(parents)} {len(operations)}")
        for vertex, parent in enumerate(parents):
            lines.append(f"{0 if parent == -1 else parent + 1} {initial[vertex]}")
        output.append(f"Case {test}:")
        for operation, employee in operations:
            lines.append(f"{operation} {employee + 1}")
            vertices = subtree[employee]
            if operation == 1:
                output.append(str(sum(salary[v] for v in vertices)))
            else:
                delta = min(1000, min(salary[v] for v in vertices))
                for v in vertices:
                    salary[v] += delta
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(output) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[([-1], [0], [(1, 0), (2, 0), (1, 0), (2, 0), (1, 0)])]]
    rng = random.Random(730012)
    for _ in range(9):
        groups = []
        for _ in range(rng.randint(1, 3)):
            n = rng.randint(2, 16)
            parents = [-1] + [rng.randrange(v) for v in range(1, n)]
            salary = [rng.randint(0, 2500) for _ in range(n)]
            operations = [(1 if rng.randrange(3) == 0 else 2, rng.randrange(n))
                          for _ in range(40)]
            groups.append((parents, salary, operations))
        cases.append(groups)
    for index, groups in enumerate(cases):
        write_case(out_dir, index, groups)


if __name__ == "__main__":
    main()
