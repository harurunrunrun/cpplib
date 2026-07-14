#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from collections import Counter
from pathlib import Path


MORSE_CODE = (
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
)


def encode(word: str) -> str:
    return "".join(MORSE_CODE[ord(character) - ord("A")] for character in word)


def solve_independent(signal: str, words: list[str]) -> int:
    encoded_words = Counter(encode(word) for word in words)
    ways = [0] * (len(signal) + 1)
    ways[0] = 1
    for start in range(len(signal)):
        if ways[start] == 0:
            continue
        for encoded, multiplicity in encoded_words.items():
            if signal.startswith(encoded, start):
                ways[start + len(encoded)] += ways[start] * multiplicity
    return ways[-1]


def validate_dataset(signal: str, words: list[str]) -> None:
    assert 1 <= len(signal) <= 10_000
    assert all(character in ".-" for character in signal)
    assert 1 <= len(words) <= 10_000
    assert len(set(words)) == len(words)
    assert all(
        1 <= len(word) <= 20
        and all("A" <= character <= "Z" for character in word)
        for word in words
    )


def write_case(
    out_dir: Path,
    name: str,
    datasets: list[tuple[str, list[str]]],
    answers: list[int] | None = None,
) -> None:
    assert 1 <= len(datasets) <= 20
    for signal, words in datasets:
        validate_dataset(signal, words)
    if answers is None:
        answers = [solve_independent(signal, words) for signal, words in datasets]
    assert len(answers) == len(datasets)
    assert all(0 <= answer <= 2_000_000_000 for answer in answers)

    input_lines = [str(len(datasets))]
    for signal, words in datasets:
        input_lines.extend((signal, str(len(words)), *words))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(map(str, answers)) + "\n",
        encoding="ascii",
    )


def base26_word(value: int) -> str:
    suffix = []
    for _ in range(19):
        suffix.append(chr(ord("A") + value % 26))
        value //= 26
    return "T" + "".join(suffix)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        (
            ".---.--.-.-.-.---...-.---.",
            ["AT", "TACK", "TICK", "ATTACK", "DAWN", "DUSK"],
        ),
        (".-", ["A", "ET"]),
        (".-.-", ["A", "ET", "AA", "ETET"]),
        ("....", ["E", "I", "S", "H"]),
        ("...", ["T", "M", "O"]),
        (encode("SOSHELP"), ["SOS", "HELP", "SO", "SHE", "LP"]),
        ("." * 45, ["E", "I"]),
    ]
    write_case(out_dir, "fixed", fixed)

    source = random.Random(840084)
    random_datasets: list[tuple[str, list[str]]] = []
    while len(random_datasets) < 20:
        alphabet = "ABCDE" if len(random_datasets) < 10 else "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        word_count = source.randint(1, 18)
        words: set[str] = set()
        while len(words) < word_count:
            words.add("".join(
                source.choice(alphabet) for _ in range(source.randint(1, 5))
            ))
        dictionary = sorted(words)
        signal = "".join(
            encode(source.choice(dictionary)) for _ in range(source.randint(1, 8))
        )
        if source.randrange(3) == 0:
            position = source.randrange(len(signal))
            replacement = "." if signal[position] == "-" else "-"
            signal = signal[:position] + replacement + signal[position + 1:]
        answer = solve_independent(signal, dictionary)
        if answer <= 2_000_000_000:
            random_datasets.append((signal, dictionary))
    write_case(out_dir, "random", random_datasets)

    maximum_dataset_count = []
    maximum_dataset_answers = []
    for index in range(20):
        word = chr(ord("A") + index)
        maximum_dataset_count.append((encode(word) * (index + 1), [word]))
        maximum_dataset_answers.append(1)
    write_case(
        out_dir,
        "max_dataset_count",
        maximum_dataset_count,
        maximum_dataset_answers,
    )

    maximum_dictionary = ["E"] + [base26_word(index) for index in range(9_999)]
    write_case(
        out_dir,
        "max_dictionary_signal_and_word_length",
        [("." * 10_000, maximum_dictionary)],
        [1],
    )
    write_case(
        out_dir,
        "max_encoded_word_length",
        [("." * 80, ["H" * 20])],
        [1],
    )


if __name__ == "__main__":
    main()
