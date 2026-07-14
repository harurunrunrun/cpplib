#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def lcp(left, right):
    length = 0
    while length < len(left) and length < len(right) and left[length] == right[length]:
        length += 1
    return length


def write_case(out_dir, index, strings):
    answer = []
    for i, value in enumerate(strings):
        answer.append(max(
            (lcp(value, other) for j, other in enumerate(strings) if i != j),
            default=0,
        ))
    (out_dir / f"case_{index:02d}.in").write_text(
        "\n".join([str(len(strings)), *strings]) + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(map(str, answer)) + ("\n" if answer else ""), encoding="utf-8"
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        [],
        ["alone"],
        ["a", "b"],
        ["abc", "abc", "abd", "ab", "z"],
        ["aaaaaaaa", "aaaaaaab", "aaa", "baaaaaaa"],
    ]
    rng = random.Random(2870501)
    alphabet = "abcd"
    for n in range(2, 31):
        strings = [
            "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 24)))
            for _ in range(n)
        ]
        if n % 5 == 0:
            strings[-1] = strings[0]
        cases.append(strings)
    for index, strings in enumerate(cases):
        write_case(out_dir, index, strings)


if __name__ == "__main__":
    main()
