#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def solve_query(values, x1, y1, x2, y2):
    answer = -10**30
    for left in range(x1, y1 + 1):
        total = 0
        for right in range(left, y2 + 1):
            total += values[right]
            if right >= x2:
                answer = max(answer, total)
    return answer


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(5005)
    cases = []
    for values in ([-8], [-5, -2, -9, -1, -7], [4, -6, 3, 5, -2, 7]):
        n = len(values)
        queries = [(0, n - 1, 0, n - 1)]
        if n > 1:
            queries.extend([(0, 0, 1, n - 1), (0, n - 1, n - 1, n - 1)])
        for _ in range(25):
            x1 = rng.randrange(n)
            x2 = rng.randrange(x1, n)
            y1 = rng.randrange(x1, n)
            y2 = rng.randrange(max(x2, y1), n)
            queries.append((x1, y1, x2, y2))
        cases.append((values, queries))

    lines = [str(len(cases))]
    answers = []
    for values, queries in cases:
        lines.extend((str(len(values)), " ".join(map(str, values)), str(len(queries))))
        for query in queries:
            lines.append(" ".join(str(value + 1) for value in query))
            answers.append(str(solve_query(values, *query)))
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
