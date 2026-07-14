#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import itertools
import random
from collections import deque
from pathlib import Path


PROBLEM = globals().get("PROBLEM")


def write_case(out_dir: Path, name: str, input_text: str, output_text: str) -> None:
    (out_dir / f"{name}.in").write_text(input_text, encoding="ascii")
    (out_dir / f"{name}.out").write_text(output_text, encoding="ascii")


def reachability(size: int, edges: list[tuple[int, int]]) -> list[list[bool]]:
    reach = [[False] * size for _ in range(size)]
    for vertex in range(size):
        reach[vertex][vertex] = True
    for left, right in edges:
        reach[left][right] = True
    for middle in range(size):
        for left in range(size):
            if not reach[left][middle]:
                continue
            for right in range(size):
                reach[left][right] |= reach[middle][right]
    return reach


def generate_bytese1(out_dir: Path) -> None:
    Case = tuple[list[list[int]], int, int, int]

    def solve(case: Case) -> str:
        grid, target_row, target_column, budget = case
        rows, columns = len(grid), len(grid[0])
        distance = [[10**30] * columns for _ in range(rows)]
        distance[0][0] = grid[0][0]
        queue = [(grid[0][0], 0, 0)]
        while queue:
            current, row, column = heapq.heappop(queue)
            if current != distance[row][column]:
                continue
            for dr, dc in ((-1, 0), (0, 1), (1, 0), (0, -1)):
                next_row, next_column = row + dr, column + dc
                if not (0 <= next_row < rows and 0 <= next_column < columns):
                    continue
                candidate = current + grid[next_row][next_column]
                if candidate < distance[next_row][next_column]:
                    distance[next_row][next_column] = candidate
                    heapq.heappush(queue, (candidate, next_row, next_column))
        cost = distance[target_row][target_column]
        return f"YES\n{budget - cost}\n" if cost <= budget else "NO\n"

    def render(cases: list[Case]) -> tuple[str, str]:
        lines = [str(len(cases))]
        answers: list[str] = []
        for grid, row, column, budget in cases:
            lines.append(f"{len(grid)} {len(grid[0])}")
            lines.extend(" ".join(map(str, values)) for values in grid)
            lines.append(f"{row + 1} {column + 1} {budget}")
            answers.append(solve((grid, row, column, budget)))
        return "\n".join(lines) + "\n", "".join(answers)

    fixed: list[Case] = [
        ([[7]], 0, 0, 7),
        ([[7]], 0, 0, 6),
        ([[1, 9, 1], [1, 1, 1]], 0, 2, 6),
        ([[5, 1], [8, 1], [1, 1]], 2, 1, 9),
    ]
    write_case(out_dir, "fixed", *render(fixed))
    source = random.Random(11501)
    cases: list[Case] = []
    for _ in range(36):
        rows, columns = source.randint(1, 6), source.randint(1, 6)
        grid = [[source.randint(0, 30) for _ in range(columns)] for _ in range(rows)]
        row, column = source.randrange(rows), source.randrange(columns)
        budget = source.randint(0, 250)
        cases.append((grid, row, column, budget))
    write_case(out_dir, "random", *render(cases))

    maximum_grid = [[9] * 100 for _ in range(100)]
    maximum_cost = 9 * (100 + 100 - 1)
    write_case(
        out_dir,
        "maximum",
        *render([(maximum_grid, 99, 99, maximum_cost)]),
    )


def capital_answer(size: int, edges: list[tuple[int, int]]) -> list[int]:
    reach = reachability(size, edges)
    return [vertex for vertex in range(size) if all(reach[other][vertex] for other in range(size))]


def generate_capcity(out_dir: Path) -> None:
    def render(size: int, edges: list[tuple[int, int]]) -> tuple[str, str]:
        answer = capital_answer(size, edges)
        input_text = f"{size} {len(edges)}\n" + "".join(
            f"{left + 1} {right + 1}\n" for left, right in edges
        )
        output_text = f"{len(answer)}\n" + "".join(f"{v + 1} " for v in answer) + "\n"
        return input_text, output_text

    write_case(out_dir, "single", *render(1, []))
    write_case(out_dir, "multiple_sinks", *render(5, [(0, 1), (1, 0), (2, 3)]))
    source = random.Random(11602)
    for case_index in range(20):
        size = source.randint(2, 9)
        edges = [
            (left, right)
            for left in range(size)
            for right in range(size)
            if left != right and source.randrange(5) == 0
        ]
        write_case(out_dir, f"random_{case_index:02d}", *render(size, edges))


def bottom_answer(size: int, edges: list[tuple[int, int]]) -> list[int]:
    reach = reachability(size, edges)
    return [
        vertex
        for vertex in range(size)
        if all(not reach[vertex][other] or reach[other][vertex] for other in range(size))
    ]


def generate_bottom(out_dir: Path) -> None:
    def render(cases: list[tuple[int, list[tuple[int, int]]]]) -> tuple[str, str]:
        lines: list[str] = []
        answers: list[str] = []
        for size, edges in cases:
            lines.extend([str(size), str(len(edges))])
            lines.extend(f"{left + 1} {right + 1}" for left, right in edges)
            answers.append(" ".join(str(v + 1) for v in bottom_answer(size, edges)))
        lines.append("0")
        return "\n".join(lines) + "\n", "\n".join(answers) + "\n"

    fixed = [(1, []), (4, [(0, 1), (1, 0), (1, 2), (3, 2)]), (3, [])]
    write_case(out_dir, "fixed", *render(fixed))
    source = random.Random(11703)
    cases = []
    for _ in range(30):
        size = source.randint(1, 8)
        edges = [
            (left, right)
            for left in range(size)
            for right in range(size)
            if left != right and source.randrange(5) == 0
        ]
        cases.append((size, edges))
    write_case(out_dir, "random", *render(cases))


def maximum_fun_brute(
    graph: list[list[int]], values: list[int], start: int, goal: int
) -> int:
    initial_mask = 1 << start
    queue = [(start, initial_mask)]
    seen = {(start, initial_mask)}
    answer = -1
    while queue:
        vertex, mask = queue.pop()
        if vertex == goal:
            answer = max(
                answer,
                sum(value for index, value in enumerate(values) if mask >> index & 1),
            )
        for to in graph[vertex]:
            state = (to, mask | (1 << to))
            if state not in seen:
                seen.add(state)
                queue.append(state)
    assert answer >= 0
    return answer


def generate_gooda(out_dir: Path) -> None:
    def render(
        size: int,
        edges: list[tuple[int, int]],
        values: list[int],
        start: int,
        goal: int,
    ) -> tuple[str, str]:
        graph = [[] for _ in range(size)]
        for left, right in edges:
            graph[left].append(right)
        answer = maximum_fun_brute(graph, values, start, goal)
        lines = [
            f"{size} {len(edges)} {start + 1} {goal + 1}",
            " ".join(map(str, values)),
            *(f"{left + 1} {right + 1}" for left, right in edges),
        ]
        return "\n".join(lines) + "\n", f"{answer}\n"

    write_case(
        out_dir,
        "cycle",
        *render(5, [(0, 1), (1, 2), (2, 1), (2, 4), (0, 3), (3, 4)], [2, 3, 5, 20, 7], 0, 4),
    )
    source = random.Random(11804)
    for case_index in range(24):
        size = source.randint(2, 8)
        start, goal = 0, size - 1
        edges = [(vertex, vertex + 1) for vertex in range(size - 1)]
        used = set(edges)
        for left in range(size):
            for right in range(size):
                if left != right and (left, right) not in used and source.randrange(5) == 0:
                    edges.append((left, right))
                    used.add((left, right))
        values = [source.randint(0, 40) for _ in range(size)]
        write_case(out_dir, f"random_{case_index:02d}", *render(size, edges, values, start, goal))


def scc_count_brute(size: int, edges: list[tuple[int, int]]) -> int:
    reach = reachability(size, edges)
    remaining = set(range(size))
    count = 0
    while remaining:
        first = next(iter(remaining))
        component = {v for v in remaining if reach[first][v] and reach[v][first]}
        remaining -= component
        count += 1
    return count


def generate_tjalg(out_dir: Path) -> None:
    def render(size: int, edges: list[tuple[int, int]]) -> tuple[str, str]:
        lines = [f"{size} {len(edges)}", *(f"{a + 1} {b + 1}" for a, b in edges)]
        return "\n".join(lines) + "\n", f"{scc_count_brute(size, edges)}\n"

    write_case(out_dir, "single", *render(1, []))
    source = random.Random(11905)
    for case_index in range(24):
        size = source.randint(1, 9)
        edges = [(a, b) for a in range(size) for b in range(size) if a != b and source.randrange(5) == 0]
        write_case(out_dir, f"random_{case_index:02d}", *render(size, edges))


def generate_maketree(out_dir: Path) -> None:
    def render(order: list[int]) -> tuple[str, str]:
        size = len(order)
        constraints = [[] for _ in range(size)]
        for index in range(size - 1):
            constraints[order[index]].append(order[index + 1])
        lines = [f"{size} {size}"]
        for children in constraints:
            lines.append(f"{len(children)}" + "".join(f" {child + 1}" for child in children))
        parent = [-1] * size
        for index in range(1, size):
            parent[order[index]] = order[index - 1]
        return "\n".join(lines) + "\n", "".join(f"{boss + 1}\n" for boss in parent)

    write_case(out_dir, "single", *render([0]))
    source = random.Random(12006)
    for case_index in range(20):
        size = source.randint(2, 20)
        order = list(range(size))
        source.shuffle(order)
        write_case(out_dir, f"random_{case_index:02d}", *render(order))


def lexicographical_topological_brute(
    size: int, edges: list[tuple[int, int]]
) -> tuple[int, ...] | None:
    for order in itertools.permutations(range(size)):
        position = [0] * size
        for index, vertex in enumerate(order):
            position[vertex] = index
        if all(position[left] < position[right] for left, right in edges):
            return order
    return None


def generate_toposort(out_dir: Path) -> None:
    def render(size: int, edges: list[tuple[int, int]]) -> tuple[str, str]:
        answer = lexicographical_topological_brute(size, edges)
        lines = [f"{size} {len(edges)}", *(f"{a + 1} {b + 1}" for a, b in edges)]
        output = "Sandro fails.\n" if answer is None else "".join(f"{v + 1} " for v in answer) + "\n"
        return "\n".join(lines) + "\n", output

    write_case(out_dir, "cycle", *render(3, [(0, 1), (1, 2), (2, 0)]))
    source = random.Random(12107)
    for case_index in range(20):
        size = source.randint(1, 8)
        edges = [(a, b) for a in range(size) for b in range(size) if a != b and source.randrange(7) == 0]
        write_case(out_dir, f"random_{case_index:02d}", *render(size, edges))


def component_count(
    size: int, edges: list[tuple[int, int]], removed_vertex: int = -1, removed_edge: int = -1
) -> int:
    graph = [[] for _ in range(size)]
    for index, (left, right) in enumerate(edges):
        if index == removed_edge or left == removed_vertex or right == removed_vertex:
            continue
        graph[left].append(right)
        graph[right].append(left)
    seen = [False] * size
    if removed_vertex >= 0:
        seen[removed_vertex] = True
    count = 0
    for start in range(size):
        if seen[start]:
            continue
        count += 1
        seen[start] = True
        queue = [start]
        while queue:
            vertex = queue.pop()
            for to in graph[vertex]:
                if not seen[to]:
                    seen[to] = True
                    queue.append(to)
    return count


def articulation_count_brute(size: int, edges: list[tuple[int, int]]) -> int:
    base = component_count(size, edges)
    return sum(component_count(size, edges, vertex) > base for vertex in range(size))


def generate_submerge(out_dir: Path) -> None:
    def render(cases: list[tuple[int, list[tuple[int, int]]]]) -> tuple[str, str]:
        lines: list[str] = []
        answers: list[str] = []
        for size, edges in cases:
            lines.append(f"{size} {len(edges)}")
            lines.extend(f"{a + 1} {b + 1}" for a, b in edges)
            answers.append(str(articulation_count_brute(size, edges)))
        lines.append("0 0")
        return "\n".join(lines) + "\n", "\n".join(answers) + "\n"

    source = random.Random(12208)
    cases = [(1, []), (5, [(0, 1), (1, 2), (1, 3), (3, 4)])]
    for _ in range(35):
        size = source.randint(1, 9)
        edges = [(a, b) for a in range(size) for b in range(a + 1, size) if source.randrange(4) == 0]
        cases.append((size, edges))
    write_case(out_dir, "mixed", *render(cases))


def non_crossing_bridges_brute(first: list[int], second: list[int]) -> int:
    size = len(first)
    answer = 0
    for mask in range(1 << size):
        if mask.bit_count() <= answer:
            continue
        selected = [index for index in range(size) if mask >> index & 1]
        if all(
            (first[left] - first[right]) * (second[left] - second[right]) >= 0
            for left, right in itertools.combinations(selected, 2)
        ):
            answer = len(selected)
    return answer


def generate_bridge(out_dir: Path) -> None:
    def render(cases: list[tuple[list[int], list[int]]]) -> tuple[str, str]:
        lines = [str(len(cases))]
        answers: list[str] = []
        for first, second in cases:
            lines.extend([str(len(first)), " ".join(map(str, first)), " ".join(map(str, second))])
            answers.append(str(non_crossing_bridges_brute(first, second)))
        return "\n".join(lines) + "\n", "\n".join(answers) + "\n"

    source = random.Random(12309)
    cases = [([1], [1]), ([1, 1, 2], [3, 1, 2]), ([1, 2, 3], [3, 2, 1])]
    for _ in range(35):
        size = source.randint(1, 10)
        cases.append(([source.randint(-5, 5) for _ in range(size)], [source.randint(-5, 5) for _ in range(size)]))
    write_case(out_dir, "mixed", *render(cases))


def bridge_edges_brute(size: int, edges: list[tuple[int, int]]) -> list[tuple[int, int]]:
    base = component_count(size, edges)
    answer = []
    for index, (left, right) in enumerate(edges):
        if component_count(size, edges, removed_edge=index) > base:
            answer.append(tuple(sorted((left, right))))
    return sorted(answer)


def generate_ec_p(out_dir: Path) -> None:
    def render(cases: list[tuple[int, list[tuple[int, int]]]]) -> tuple[str, str]:
        lines = [str(len(cases))]
        answers: list[str] = []
        for test_index, (size, edges) in enumerate(cases, 1):
            lines.append(f"{size} {len(edges)}")
            lines.extend(f"{a + 1} {b + 1}" for a, b in edges)
            critical = bridge_edges_brute(size, edges)
            answers.append(f"Caso #{test_index}\n")
            if critical:
                answers.append(f"{len(critical)}\n")
                answers.extend(f"{a + 1} {b + 1}\n" for a, b in critical)
            else:
                answers.append("Sin bloqueos\n")
        return "\n".join(lines) + "\n", "".join(answers)

    source = random.Random(12410)
    cases = [(1, []), (3, [(0, 1), (1, 2), (2, 0)]), (4, [(0, 1), (1, 2), (1, 3)])]
    for _ in range(28):
        size = source.randint(1, 9)
        edges = [(a, b) for a in range(size) for b in range(a + 1, size) if source.randrange(4) == 0]
        cases.append((size, edges))
    write_case(out_dir, "mixed", *render(cases))


def component_sums_brute(
    values: list[int], edges: list[tuple[int, int]]
) -> list[int]:
    size = len(values)
    graph = [[] for _ in range(size)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    seen = [False] * size
    sums = []
    for start in range(size):
        if seen[start]:
            continue
        seen[start] = True
        queue = [start]
        total = 0
        while queue:
            vertex = queue.pop()
            total += values[vertex]
            for to in graph[vertex]:
                if not seen[to]:
                    seen[to] = True
                    queue.append(to)
        sums.append(total)
    return sorted(sums)


def generate_iapcr2f(out_dir: Path) -> None:
    Case = tuple[list[int], list[tuple[int, int]]]

    def render(cases: list[Case]) -> tuple[str, str]:
        lines = [str(len(cases))]
        answers: list[str] = []
        for test_index, (values, edges) in enumerate(cases, 1):
            lines.extend([f"{len(values)} {len(edges)}", " ".join(map(str, values))])
            lines.extend(f"{a + 1} {b + 1}" for a, b in edges)
            sums = component_sums_brute(values, edges)
            answers.extend([f"Case {test_index}: {len(sums)}", " ".join(map(str, sums))])
        return "\n".join(lines) + "\n", "\n".join(answers) + "\n"

    source = random.Random(12511)
    cases: list[Case] = [([10], []), ([1, 2, 3, 4], [(0, 1), (2, 3)])]
    for _ in range(35):
        size = source.randint(1, 10)
        values = [source.randint(1, 10**6) for _ in range(size)]
        edges = [(a, b) for a in range(size) for b in range(a + 1, size) if source.randrange(5) == 0]
        cases.append((values, edges))
    write_case(out_dir, "mixed", *render(cases))


def guard_coverage_brute(
    size: int, edges: list[tuple[int, int]], guards: list[tuple[int, int]]
) -> bool:
    graph = [[] for _ in range(size)]
    for left, right in edges:
        graph[left].append(right)
        graph[right].append(left)
    covered = [0] * size
    for base, strength in guards:
        distance = [-1] * size
        distance[base] = 0
        queue = deque([base])
        while queue:
            vertex = queue.popleft()
            covered[vertex] += 1
            if distance[vertex] == strength:
                continue
            for to in graph[vertex]:
                if distance[to] == -1:
                    distance[to] = distance[vertex] + 1
                    queue.append(to)
    return all(count == 1 for count in covered)


def generate_akbar(out_dir: Path) -> None:
    Case = tuple[int, list[tuple[int, int]], list[tuple[int, int]]]

    def render(cases: list[Case]) -> tuple[str, str]:
        lines = [str(len(cases))]
        answers: list[str] = []
        for size, edges, guards in cases:
            lines.append(f"{size} {len(edges)} {len(guards)}")
            lines.extend(f"{a + 1} {b + 1}" for a, b in edges)
            lines.extend(f"{base + 1} {strength}" for base, strength in guards)
            answers.append("Yes" if guard_coverage_brute(size, edges, guards) else "No")
        return "\n".join(lines) + "\n", "\n".join(answers) + "\n"

    source = random.Random(12612)
    cases: list[Case] = [
        (1, [], [(0, 0)]),
        (2, [(0, 1)], [(0, 1)]),
        (2, [(0, 1)], [(0, 1), (1, 0)]),
        (3, [(0, 1)], [(0, 1)]),
    ]
    for _ in range(50):
        size = source.randint(1, 10)
        edges = [(a, b) for a in range(size) for b in range(a + 1, size) if source.randrange(4) == 0]
        guard_count = source.randint(1, min(size, 4))
        bases = source.sample(range(size), guard_count)
        guards = [(base, source.randint(0, 4)) for base in bases]
        cases.append((size, edges, guards))
    write_case(out_dir, "mixed", *render(cases))


GENERATORS = {
    "bytese1": generate_bytese1,
    "capcity": generate_capcity,
    "bottom": generate_bottom,
    "gooda": generate_gooda,
    "tjalg": generate_tjalg,
    "maketree": generate_maketree,
    "toposort": generate_toposort,
    "submerge": generate_submerge,
    "bridge": generate_bridge,
    "ec_p": generate_ec_p,
    "iapcr2f": generate_iapcr2f,
    "akbar": generate_akbar,
}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    if PROBLEM not in GENERATORS:
        raise SystemExit(f"unsupported problem: {PROBLEM!r}")
    args.out_dir.mkdir(parents=True, exist_ok=True)
    GENERATORS[PROBLEM](args.out_dir)


if __name__ == "__main__":
    main()
