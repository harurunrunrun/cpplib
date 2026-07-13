#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import math
from pathlib import Path


def best(points, query):
    query_norm = math.sqrt(sum(value * value for value in query))
    values = []
    for index, point in enumerate(points):
        norm = math.sqrt(sum(value * value for value in point))
        similarity = sum(a * b for a, b in zip(point, query)) / (norm * query_norm)
        values.append((-similarity, index))
    return min(values)[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        ([(1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1), (1, 1, 1, 1)], [0, 2, 4], 8, 9, 1001),
        ([(1, 2, 3, 4), (2, 4, 6, 8), (-1, -2, -3, -4), (3, -1, 7, 2)], [1, 2, 3], 12, 7, 99),
        ([(i + 1, (i * 7) % 17 - 8, (i * 11) % 23 - 11, (i * 13) % 29 - 14) for i in range(50)], [3, 17, 49], 16, 10, 2026),
    ]
    for case_id, (points, query_indices, tables, bits, seed) in enumerate(cases):
        queries = [points[index] for index in query_indices]
        lines = [f"4 {len(points)} {len(queries)} {tables} {bits} {seed}"]
        lines.extend(" ".join(map(str, point)) for point in points)
        lines.extend(" ".join(map(str, query)) for query in queries)
        answers = [f"{best(points, query)} 1" for query in queries]
        (out_dir / f"case_{case_id:02}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
        (out_dir / f"case_{case_id:02}.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
