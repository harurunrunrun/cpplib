#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


def answer(points, query):
    if not points:
        return "none"
    distance, index = min(
        (sum((point[d] - query[d]) ** 2 for d in range(3)), index)
        for index, point in enumerate(points)
    )
    return f"{index} {distance}"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        ([(i * 7 % 31, i * 11 % 37, i * 13 % 41) for i in range(60)], [(0, 0, 0), (15, 16, 17), (30, 36, 40)]),
        ([(1, 2, 3), (1, 2, 3), (-1000000000, 0, 0), (1000000000, 0, 0)], [(1, 2, 3), (0, 0, 0)]),
        ([], [(1, 2, 3), (0, 0, 0)]),
    ]
    for case_id, (points, queries) in enumerate(cases):
        lines = [f"{len(points)} {len(queries)}"]
        lines.extend(" ".join(map(str, point)) for point in points)
        lines.extend(" ".join(map(str, query)) for query in queries)
        expected = "\n".join(answer(points, query) for query in queries) + "\n"
        (out_dir / f"case_{case_id:02}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
        (out_dir / f"case_{case_id:02}.out").write_text(expected, encoding="utf-8")


if __name__ == "__main__":
    main()
