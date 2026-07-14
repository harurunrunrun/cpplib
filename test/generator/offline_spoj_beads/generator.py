#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import itertools
import random
import string
from pathlib import Path


Case = tuple[str, int]


def brute_minimum_rotation(text: str) -> int:
    assert text
    return min(
        range(len(text)),
        key=lambda index: (text[index:] + text[:index], index),
    )


def write_suite(out_dir: Path, stem: str, cases: list[Case]) -> None:
    assert cases
    assert all(1 <= len(text) <= 10_000 for text, _ in cases)
    assert all(set(text) <= set(string.ascii_lowercase) for text, _ in cases)
    (out_dir / f"{stem}.in").write_text(
        str(len(cases)) + "\n" + "\n".join(text for text, _ in cases) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{stem}.out").write_text(
        "".join(f"{index + 1}\n" for _, index in cases),
        encoding="ascii",
    )


def official_sample() -> list[Case]:
    texts = ["helloworld", "amandamanda", "dontcallmebfu", "aaabaaa"]
    expected = [9, 10, 5, 4]
    assert [brute_minimum_rotation(text) for text in texts] == expected
    return list(zip(texts, expected))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_suite(out_dir, "official_sample", official_sample())

    periodic_texts = [
        "a",
        "aa",
        "a" * 37,
        "abab",
        "baba",
        "abababababab",
        "babababababa",
        "abcabcabc",
        "bcabcabca",
        "cabcaabcabca",
        "zzzzazzzzazzzza",
    ]
    write_suite(
        out_dir,
        "periodic_ties",
        [(text, brute_minimum_rotation(text)) for text in periodic_texts],
    )

    exhaustive_texts = [
        "".join(characters)
        for length in range(1, 10)
        for characters in itertools.product("ab", repeat=length)
    ]
    write_suite(
        out_dir,
        "exhaustive_binary",
        [(text, brute_minimum_rotation(text)) for text in exhaustive_texts],
    )

    rng = random.Random(202607140093)
    random_texts: list[str] = []
    for _ in range(500):
        length = rng.randrange(1, 101)
        alphabet = string.ascii_lowercase[: rng.randrange(1, 9)]
        random_texts.append(
            "".join(rng.choice(alphabet) for _ in range(length))
        )
    write_suite(
        out_dir,
        "random_bruteforce",
        [(text, brute_minimum_rotation(text)) for text in random_texts],
    )

    maximum_length = 10_000
    write_suite(
        out_dir,
        "maximum_length",
        [
            ("a" * maximum_length, 0),
            ("ab" * (maximum_length // 2), 0),
            ("ba" * (maximum_length // 2), 1),
            ("z" * (maximum_length - 1) + "a", maximum_length - 1),
            ("b" + "a" * (maximum_length - 1), 1),
            ("c" * (maximum_length // 2) + "b" * (maximum_length // 2),
             maximum_length // 2),
        ],
    )

    # The official statement does not publish an upper bound for the number
    # of cases.  Keep a large parser/output stress suite in addition to the
    # specified maximum necklace length above.
    many_cases = [
        (string.ascii_lowercase[index % 26], 0) for index in range(10_000)
    ]
    write_suite(out_dir, "large_test_count", many_cases)


if __name__ == "__main__":
    main()
