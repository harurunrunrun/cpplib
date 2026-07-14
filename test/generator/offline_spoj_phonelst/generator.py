#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_TEST_COUNT = 40
MAX_PHONE_COUNT = 10_000
MAX_DIGITS = 10


def is_prefix_free_brute(numbers: list[str]) -> bool:
    return all(
        first_index == second_index or not second.startswith(first)
        for first_index, first in enumerate(numbers)
        for second_index, second in enumerate(numbers)
    )


def validate(cases: list[list[str]]) -> None:
    assert 1 <= len(cases) <= MAX_TEST_COUNT
    assert all(1 <= len(numbers) <= MAX_PHONE_COUNT for numbers in cases)
    assert all(
        1 <= len(number) <= MAX_DIGITS
        and all("0" <= digit <= "9" for digit in number)
        for numbers in cases
        for number in numbers
    )


def write_case(
    out_dir: Path,
    name: str,
    cases: list[list[str]],
    answers: list[bool] | None = None,
) -> None:
    validate(cases)
    if answers is None:
        answers = [is_prefix_free_brute(numbers) for numbers in cases]
    assert len(answers) == len(cases)

    input_lines = [str(len(cases))]
    for numbers in cases:
        input_lines.append(str(len(numbers)))
        input_lines.extend(numbers)
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join("YES" if answer else "NO" for answer in answers) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        "official",
        [
            ["911", "97625999", "91125426"],
            ["113", "12340", "123440", "12345", "98346"],
        ],
        [False, True],
    )
    write_case(
        out_dir,
        "duplicates_and_insertion_orders",
        [
            ["911", "91125426"],
            ["91125426", "911"],
            ["123", "123"],
            ["12", "123", "456"],
            ["123", "456", "789"],
            ["0", "00"],
            ["0000000000", "9999999999"],
        ],
    )

    source = random.Random(40330091)
    for file_index in range(8):
        cases = []
        for case_index in range(MAX_TEST_COUNT):
            phone_count = source.randint(1, 35)
            numbers = [
                "".join(
                    source.choice("0123456789")
                    for _ in range(source.randint(1, MAX_DIGITS))
                )
                for _ in range(phone_count)
            ]
            if phone_count >= 2 and case_index % 3 == 0:
                longer = "".join(
                    source.choice("0123456789")
                    for _ in range(source.randint(2, MAX_DIGITS))
                )
                pair = [longer[:-1], longer]
                if case_index % 2 == 0:
                    pair.reverse()
                numbers[:2] = pair
            elif phone_count >= 2 and case_index % 3 == 1:
                numbers[1] = numbers[0]
            cases.append(numbers)
        write_case(out_dir, f"random_{file_index:02d}", cases)

    maximum_consistent = [f"{number:010d}" for number in range(MAX_PHONE_COUNT)]
    write_case(
        out_dir,
        "max_phone_count_and_digits",
        [maximum_consistent],
        [True],
    )

    late_conflict = [f"{number:010d}" for number in range(MAX_PHONE_COUNT - 2)]
    late_conflict.extend(["1234567890", "123456789"])
    write_case(
        out_dir,
        "max_phone_count_late_conflict",
        [late_conflict],
        [False],
    )

    maximum_test_cases = []
    maximum_test_answers = []
    for test_case in range(MAX_TEST_COUNT):
        if test_case % 2 == 0:
            maximum_test_cases.append([f"{test_case:010d}"])
            maximum_test_answers.append(True)
        else:
            maximum_test_cases.append(["123456789", "1234567890"])
            maximum_test_answers.append(False)
    write_case(
        out_dir,
        "max_test_count",
        maximum_test_cases,
        maximum_test_answers,
    )


if __name__ == "__main__":
    main()
