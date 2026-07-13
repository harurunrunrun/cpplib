#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import heapq
import random
from pathlib import Path


def order(weights: list[int], decreasing: bool) -> list[int]:
    result = list(range(len(weights)))
    if decreasing:
        result.sort(key=lambda item: (-weights[item], item))
    return result


def greedy(
    weights: list[int],
    part_count: int,
    decreasing: bool,
) -> tuple[list[int], list[int]]:
    queue = [(0, part) for part in range(part_count)]
    heapq.heapify(queue)
    assignment = [0] * len(weights)
    sums = [0] * part_count
    for item in order(weights, decreasing):
        current_sum, part = heapq.heappop(queue)
        assignment[item] = part
        current_sum += weights[item]
        sums[part] = current_sum
        heapq.heappush(queue, (current_sum, part))
    return assignment, sums


def round_robin(
    weights: list[int],
    part_count: int,
    decreasing: bool,
) -> tuple[list[int], list[int]]:
    assignment = [0] * len(weights)
    sums = [0] * part_count
    for position, item in enumerate(order(weights, decreasing)):
        part = position % part_count
        assignment[item] = part
        sums[part] += weights[item]
    return assignment, sums


def largest_differencing(weights: list[int]) -> tuple[list[int], list[int]]:
    if not weights:
        return [], [0, 0]
    nodes: list[tuple[int, int, int | None]] = [
        (0, 0, item) for item in range(len(weights))
    ]
    queue = [(-weight, item) for item, weight in enumerate(weights)]
    heapq.heapify(queue)
    while len(queue) > 1:
        neg_first, first = heapq.heappop(queue)
        neg_second, second = heapq.heappop(queue)
        node = len(nodes)
        nodes.append((first, second, None))
        difference = (-neg_first) - (-neg_second)
        heapq.heappush(queue, (-difference, node))

    assignment = [0] * len(weights)
    stack = [(heapq.heappop(queue)[1], False)]
    while stack:
        node, reversed_side = stack.pop()
        left, right, item = nodes[node]
        if item is not None:
            assignment[item] = int(reversed_side)
        else:
            stack.append((right, not reversed_side))
            stack.append((left, reversed_side))
    sums = [0, 0]
    for item, part in enumerate(assignment):
        sums[part] += weights[item]
    return assignment, sums


def format_result(result: tuple[list[int], list[int]]) -> str:
    assignment, sums = result
    return " ".join(map(str, [len(sums), *assignment, *sums]))


def write_case(path: Path, cases: list[tuple[list[int], int]]) -> None:
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for weights, part_count in cases:
        input_lines.extend([
            f"{len(weights)} {part_count}",
            " ".join(map(str, weights)),
        ])
        load = greedy(weights, part_count, False)
        load_decreasing = greedy(weights, part_count, True)
        multiway = greedy(weights, part_count, True)
        rr = round_robin(weights, part_count, False)
        rr_decreasing = round_robin(weights, part_count, True)
        greedy_two_way = greedy(weights, 2, True)
        differencing = largest_differencing(weights)
        output_lines.extend(map(format_result, [
            load,
            load_decreasing,
            multiway,
            rr,
            rr_decreasing,
            greedy_two_way,
            differencing,
            differencing,
        ]))
    path.with_suffix(".in").write_text(
        "\n".join(input_lines) + "\n", encoding="utf-8"
    )
    path.with_suffix(".out").write_text(
        "\n".join(output_lines) + "\n", encoding="utf-8"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    maximum = (1 << 64) - 1
    edge_cases: list[tuple[list[int], int]] = [
        ([], 1),
        ([], 8),
        ([0], 1),
        ([0, 0, 0, 0, 0], 3),
        ([5, 5, 5, 5, 5, 5], 2),
        ([8, 7, 6, 5, 4, 3, 2, 1], 3),
        ([maximum], 4),
        ([maximum, maximum], 2),
        ([(1 << 63), (1 << 62), (1 << 62) - 1], 4),
        ([10, 0, 10, 0, 10, 0], 12),
    ]
    write_case(out_dir / "case_00", edge_cases)

    rng = random.Random(2026071319)
    random_cases: list[tuple[list[int], int]] = []
    for _ in range(70):
        item_count = rng.randrange(0, 121)
        part_count = rng.randrange(1, 18)
        if rng.randrange(4) == 0:
            choices = [0, 1, 2, 3, 5, 8, 13, 21]
            weights = [rng.choice(choices) for _ in range(item_count)]
        else:
            weights = [rng.randrange(0, 10**12) for _ in range(item_count)]
        random_cases.append((weights, part_count))
    write_case(out_dir / "case_01", random_cases)


if __name__ == "__main__":
    main()
