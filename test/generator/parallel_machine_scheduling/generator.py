#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import functools
import heapq
import random
from pathlib import Path


def makespan(values: list[int], machine_count: int, order: list[int]) -> int:
    heap = [(0, machine) for machine in range(machine_count)]
    heapq.heapify(heap)
    answer = 0
    for job in order:
        load, machine = heapq.heappop(heap)
        load += values[job]
        answer = max(answer, load)
        heapq.heappush(heap, (load, machine))
    return answer


def wspt_order(times: list[int], weights: list[int]) -> list[int]:
    def compare(first: int, second: int) -> int:
        left = times[first] * weights[second]
        right = times[second] * weights[first]
        return (left > right) - (left < right)

    return sorted(range(len(times)), key=functools.cmp_to_key(compare))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071311)
    cases: list[tuple[list[int], int, list[int], list[int]]] = [
        ([], 3, [], []),
        ([5], 1, [7], [0]),
        ([8, 1, 7, 3, 9, 2], 3, [2, 5, 3, 7, 11, 13], [4, 4, 2, 9, 2, 9]),
        ([4_000_000_000, 3_999_999_999, 1], 2,
         [4_000_000_000, 3_999_999_997, 1], [8, 7, 6]),
    ]
    for _ in range(36):
        size = rng.randrange(0, 50)
        machines = rng.randrange(1, 9)
        values = [rng.randrange(0, 10_000) for _ in range(size)]
        weights = [rng.randrange(1, 10_000) for _ in range(size)]
        due = [rng.randrange(-10_000, 20_000) for _ in range(size)]
        cases.append((values, machines, weights, due))

    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for values, machines, weights, due in cases:
        size = len(values)
        input_lines.extend([
            f"{size} {machines}",
            " ".join(map(str, values)),
            " ".join(map(str, weights)),
            " ".join(map(str, due)),
        ])
        fifo = list(range(size))
        lifo = list(reversed(fifo))
        lpt = sorted(fifo, key=lambda job: -values[job])
        spt = sorted(fifo, key=lambda job: values[job])
        output_lines.append(" ".join(map(str, [
            makespan(values, machines, fifo),
            makespan(values, machines, lifo),
            makespan(values, machines, lpt),
            makespan(values, machines, spt),
        ])))
        wspt = wspt_order(values, weights)
        edd = sorted(fifo, key=lambda job: due[job])
        output_lines.append(" ".join(map(str, [size, *wspt])))
        output_lines.append(" ".join(map(str, [size, *edd])))

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
