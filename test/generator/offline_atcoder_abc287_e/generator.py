#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def lcp(a, b):
    length = 0
    while length < len(a) and length < len(b) and a[length] == b[length]:
        length += 1
    return length


def write_case(out_dir, index, strings):
    answer = [max((lcp(value, other) for j, other in enumerate(strings) if i != j), default=0)
              for i, value in enumerate(strings)]
    lines = [str(len(strings))] + strings
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(map(str, answer)) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [["a", "b"], ["abc", "abd", "ab", "xyz"],
             ["aaaaaaaa", "aaaaaaab", "aaaabaaa", "baaaaaaa"]]
    rng = random.Random(28705)
    for n in range(2, 13):
        strings = set()
        while len(strings) < n:
            strings.add("".join(rng.choice("abc") for _ in range(rng.randint(1, 9))))
        cases.append(sorted(strings))
    for index, strings in enumerate(cases):
        write_case(out_dir, index, strings)


if __name__ == "__main__":
    main()
