#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def common_suffix_length(left: str, right: str) -> int:
    length = 0
    while (
        length < len(left)
        and length < len(right)
        and left[-1 - length] == right[-1 - length]
    ):
        length += 1
    return length


def solve_independent(words: list[str], query: str) -> str:
    candidates = [word for word in words if word != query]
    if not candidates:
        return "NONE"
    best_length = max(common_suffix_length(word, query) for word in candidates)
    return min(
        word
        for word in candidates
        if common_suffix_length(word, query) == best_length
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    datasets: list[tuple[list[str], list[str]]] = [
        (
            ["perfect", "rhyme", "crime", "time"],
            ["crime", "rhyme", "dime", "perfect", "zzz"],
        ),
        (["a"], ["a", "b", "aa"]),
        (["a", "ba", "cba", "aa", "aaa"], ["a", "ba", "cba", "dba"]),
    ]

    source = random.Random(27370085)
    for _ in range(100):
        word_count = source.randint(1, 18)
        words: set[str] = set()
        while len(words) < word_count:
            words.add("".join(
                source.choice("abcde")
                for _ in range(source.randint(1, 7))
            ))
        dictionary = sorted(words)
        queries = []
        for _ in range(source.randint(1, 20)):
            if source.randrange(2) == 0:
                queries.append(source.choice(dictionary))
            else:
                queries.append("".join(
                    source.choice("abcdef")
                    for _ in range(source.randint(1, 7))
                ))
        datasets.append((dictionary, queries))

    input_lines = [str(len(datasets))]
    output_lines: list[str] = []
    for words, queries in datasets:
        input_lines.append(f"{len(words)} {len(queries)}")
        input_lines.extend(words)
        input_lines.extend(queries)
        output_lines.extend(solve_independent(words, query) for query in queries)

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
