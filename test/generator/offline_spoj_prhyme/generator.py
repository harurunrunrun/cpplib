#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_WORD_COUNT = 250_000
MAX_WORD_LENGTH = 30


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
    assert candidates
    best_length = max(common_suffix_length(word, query) for word in candidates)
    return min(
        word
        for word in candidates
        if common_suffix_length(word, query) == best_length
    )


def validate(words: list[str], queries: list[str]) -> None:
    assert 1 <= len(words) <= MAX_WORD_COUNT
    assert 1 <= len(queries) <= MAX_WORD_COUNT
    assert len(set(words)) == len(words)
    assert all(
        1 <= len(word) <= MAX_WORD_LENGTH and word.isascii() and word.islower()
        for word in words
    )
    assert all(
        1 <= len(query) <= MAX_WORD_LENGTH and query.isascii() and query.islower()
        for query in queries
    )


def write_case(
    out_dir: Path,
    name: str,
    words: list[str],
    queries: list[str],
    answers: list[str] | None = None,
) -> None:
    validate(words, queries)
    if answers is None:
        answers = [solve_independent(words, query) for query in queries]
    assert len(answers) == len(queries)
    assert all(answer in words and answer != query for answer, query in zip(answers, queries))

    input_text = "\n".join(words) + "\n\n" + "\n".join(queries) + "\n"
    (out_dir / f"{name}.in").write_text(input_text, encoding="ascii")
    (out_dir / f"{name}.out").write_text(
        "\n".join(answers) + "\n",
        encoding="ascii",
    )


def base26_word(value: int, width: int) -> str:
    characters = ["a"] * width
    for index in range(width - 1, -1, -1):
        characters[index] = chr(ord("a") + value % 26)
        value //= 26
    assert value == 0
    return "".join(characters)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        "official",
        ["perfect", "rhyme", "crime", "time"],
        ["crime", "rhyme"],
        ["time", "crime"],
    )
    fixed_words = ["a", "ba", "cba", "aa", "aaa", "baba", "z"]
    fixed_queries = ["a", "ba", "cba", "aa", "aaa", "dba", "x", "aaaa"]
    write_case(out_dir, "fixed", fixed_words, fixed_queries)

    source = random.Random(27370085)
    for case_index in range(20):
        word_count = source.randint(2, 30)
        words: set[str] = set()
        while len(words) < word_count:
            words.add("".join(
                source.choice("abcdef")
                for _ in range(source.randint(1, 8))
            ))
        dictionary = sorted(words)
        queries = []
        for _ in range(source.randint(1, 35)):
            if source.randrange(2) == 0:
                queries.append(source.choice(dictionary))
            else:
                queries.append("".join(
                    source.choice("abcdefg")
                    for _ in range(source.randint(1, 8))
                ))
        write_case(
            out_dir,
            f"random_{case_index:02d}",
            dictionary,
            queries,
        )

    maximum_dictionary = [
        base26_word(index, MAX_WORD_LENGTH - 1) + "a"
        for index in range(MAX_WORD_COUNT)
    ]
    write_case(
        out_dir,
        "max_dictionary_and_word_length",
        maximum_dictionary,
        ["z" * MAX_WORD_LENGTH],
        ["a" * MAX_WORD_LENGTH],
    )
    write_case(
        out_dir,
        "max_query_count",
        ["a", "b"],
        ["z"] * MAX_WORD_COUNT,
        ["a"] * MAX_WORD_COUNT,
    )


if __name__ == "__main__":
    main()
