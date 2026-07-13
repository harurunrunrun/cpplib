#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import heapq
from pathlib import Path


def area(a, b, c):
    return abs((b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]))


def simplify(points, target):
    n = len(points)
    previous = [i - 1 if i else n for i in range(n)]
    following = [i + 1 if i + 1 < n else n for i in range(n)]
    version = [0] * n
    alive = [True] * n
    queue = []
    def push(index):
        if index == 0 or index + 1 == n or not alive[index]:
            return
        heapq.heappush(queue, (area(points[previous[index]], points[index], points[following[index]]), index, version[index]))
    for index in range(1, n - 1):
        push(index)
    count = n
    while count > target:
        while True:
            _, index, old_version = heapq.heappop(queue)
            if alive[index] and version[index] == old_version:
                break
        left, right = previous[index], following[index]
        alive[index] = False
        count -= 1
        following[left] = right
        previous[right] = left
        if left != 0:
            version[left] += 1
            push(left)
        if right + 1 != n:
            version[right] += 1
            push(right)
    return [i for i in range(n) if alive[i]]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        (4, [(0, 0), (1, 0.1), (2, 3), (3, 3.1), (4, 0), (5, 0)]),
        (2, [(0, 0), (0, 0), (1, 0), (1, 0), (2, 0)]),
        (17, [(i, (i * 13 + i * i * 7) % 29) for i in range(80)]),
    ]
    for case_id, (target, points) in enumerate(cases):
        indices = simplify(points, target)
        lines = [f"{len(points)} {target}"] + [f"{x} {y}" for x, y in points]
        (out_dir / f"case_{case_id:02}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
        (out_dir / f"case_{case_id:02}.out").write_text(str(len(indices)) + " " + " ".join(map(str, indices)) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
