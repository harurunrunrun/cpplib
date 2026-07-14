#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from functools import lru_cache
from pathlib import Path


def solve(words: list[str]) -> bool:
    @lru_cache(maxsize=None)
    def search(mask: int, last_character: str) -> bool:
        if mask == (1 << len(words)) - 1:
            return True
        for index, word in enumerate(words):
            if mask >> index & 1:
                continue
            if not last_character or word[0] == last_character:
                if search(mask | (1 << index), word[-1]):
                    return True
        return False

    return search(0, "")


def write_case(out_dir: Path, name: str, tests: list[list[str]], answers: list[bool] | None = None) -> None:
    input_lines = [str(len(tests))]
    output_lines = []
    for index, words in enumerate(tests):
        input_lines.append(str(len(words)))
        input_lines.extend(words)
        possible = solve(words) if answers is None else answers[index]
        output_lines.append(
            "Ordering is possible." if possible else "The door cannot be opened."
        )
    (out_dir / f"{name}.in").write_text("\n".join(input_lines) + "\n", encoding="ascii")
    (out_dir / f"{name}.out").write_text("\n".join(output_lines) + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, "official", [
        ["acm", "ibm"],
        ["acm", "malform", "mouse"],
        ["ok", "ok"],
    ])
    write_case(out_dir, "basic", [
        ["aa"],
        ["ab", "bc", "ca"],
        ["ab", "ba", "xy", "yx"],
        ["ab", "ac"],
    ])

    source = random.Random(150)
    alphabet = "abcde"
    tests = []
    for _ in range(60):
        words = []
        for _ in range(source.randint(1, 8)):
            first = source.choice(alphabet)
            last = source.choice(alphabet)
            middle = "".join(source.choice(alphabet) for _ in range(source.randint(0, 4)))
            words.append(first + middle + last)
        tests.append(words)
    write_case(out_dir, "random", tests)

    maximum = ["ab" if index % 2 == 0 else "ba" for index in range(100000)]
    write_case(out_dir, "maximum", [maximum], [True])


if __name__ == "__main__":
    main()
