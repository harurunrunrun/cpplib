#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import heapq
import random
from pathlib import Path


def solve(n, edges, switches):
    graph = [[] for _ in range(2 * n)]
    for u, v, state in edges:
        a = u + state * n
        b = v + state * n
        graph[a].append((b, 1))
        graph[b].append((a, 1))
    for v in switches:
        graph[v].append((v + n, 0))
        graph[v + n].append((v, 0))
    inf = 10 ** 9
    dist = [inf] * (2 * n)
    dist[n] = 0
    queue = [(0, n)]
    while queue:
        distance, v = heapq.heappop(queue)
        if distance != dist[v]:
            continue
        for to, weight in graph[v]:
            candidate = distance + weight
            if candidate < dist[to]:
                dist[to] = candidate
                heapq.heappush(queue, (candidate, to))
    answer = min(dist[n - 1], dist[2 * n - 1])
    return -1 if answer == inf else answer


def write_case(out_dir, index, n, edges, switches):
    lines = [f"{n} {len(edges)} {len(switches)}"]
    lines += [f"{u + 1} {v + 1} {state}" for u, v, state in edges]
    lines += [str(v + 1) for v in switches]
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{solve(n, edges, switches)}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [(2, [(0, 1, 1)], []), (2, [(0, 1, 0)], []),
             (3, [(0, 1, 1), (1, 2, 0)], [1]),
             (4, [(0, 1, 0), (1, 2, 0), (2, 3, 1)], [0, 2])]
    rng = random.Random(27705)
    for n in range(2, 11):
        candidates = [(u, v, state) for u in range(n) for v in range(u + 1, n)
                      for state in range(2)]
        rng.shuffle(candidates)
        edges = candidates[:rng.randint(1, min(len(candidates), 2 * n + 3))]
        switches = [v for v in range(n) if rng.randrange(3) == 0]
        cases.append((n, edges, switches))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
