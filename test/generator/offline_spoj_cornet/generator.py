#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def distance_to_center(vertex, parent):
    distance = 0
    while parent[vertex] != vertex:
        next_vertex = parent[vertex]
        distance += abs(vertex - next_vertex) % 1000
        vertex = next_vertex
    return distance


def build_case(size, rng):
    parent = list(range(size))
    lines = [str(size)]
    answers = []
    lines.append("E 1")
    answers.append("0")
    unattached = list(range(1, size))
    rng.shuffle(unattached)
    for vertex in unattached:
        destination = rng.randrange(vertex)
        lines.append(f"I {vertex + 1} {destination + 1}")
        parent[vertex] = destination
        for query_vertex in dict.fromkeys((vertex, destination, rng.randrange(size))):
            lines.append(f"E {query_vertex + 1}")
            answers.append(str(distance_to_center(query_vertex, parent)))
    lines.append("O")
    return lines, answers


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(76009)
    cases = [build_case(1, rng), build_case(23, rng)]
    input_lines = [str(len(cases))]
    output_lines = []
    for lines, answers in cases:
        input_lines.extend(lines)
        output_lines.extend(answers)
    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n")


if __name__ == "__main__":
    main()
