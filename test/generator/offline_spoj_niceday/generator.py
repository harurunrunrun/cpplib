#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def make_case(rng, n):
    rankings = []
    for _ in range(3):
        order = list(range(n))
        rng.shuffle(order)
        rank = [0] * n
        for position, competitor in enumerate(order, 1):
            rank[competitor] = position
        rankings.append(rank)
    triples = [tuple(rankings[dimension][competitor] for dimension in range(3)) for competitor in range(n)]
    excellent = 0
    for current in triples:
        if not any(all(other[d] < current[d] for d in range(3)) for other in triples):
            excellent += 1
    rng.shuffle(triples)
    return triples, excellent


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(8100)
    cases = [make_case(rng, n) for n in (1, 2, 7, 18, 40)]
    lines = [str(len(cases))]
    answers = []
    for triples, excellent in cases:
        lines.append(str(len(triples)))
        lines.extend(" ".join(map(str, triple)) for triple in triples)
        answers.append(str(excellent))
    (out_dir / "cases.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "cases.out").write_text("\n".join(answers) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
