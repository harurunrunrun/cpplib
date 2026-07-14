#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import functools
import random
from pathlib import Path


MAX_TEST_COUNT = 1000
MAX_SELLERS = 100
MAX_BUYERS = 100
MAX_GOODS = 100

Offer = tuple[int, int, int]
Case = tuple[int, int, list[Offer]]


def brute_force_oracle(case: Case) -> int:
    """Independent exhaustive oracle for small cases."""
    seller_count, buyer_count, offers = case
    by_seller: list[list[tuple[int, int]]] = [
        [] for _ in range(seller_count)
    ]
    for seller, buyer, goods in offers:
        by_seller[seller].append((buyer, goods))

    @functools.cache
    def search(seller: int, used_buyers: int) -> int:
        if seller == seller_count:
            return 0
        answer = search(seller + 1, used_buyers)
        for buyer, goods in by_seller[seller]:
            if used_buyers >> buyer & 1:
                continue
            answer = max(
                answer,
                goods + search(seller + 1, used_buyers | (1 << buyer)),
            )
        return answer

    return search(0, 0)


def validate_case(case: Case) -> None:
    seller_count, buyer_count, offers = case
    assert 1 <= seller_count <= MAX_SELLERS
    assert 1 <= buyer_count <= MAX_BUYERS
    for seller, buyer, goods in offers:
        assert 0 <= seller < seller_count
        assert 0 <= buyer < buyer_count
        assert 1 <= goods <= MAX_GOODS


def write_suite(
    out_dir: Path,
    suite_name: str,
    cases: list[Case],
    expected: list[int] | None = None,
    *,
    final_newline: bool = True,
) -> None:
    assert 1 <= len(cases) <= MAX_TEST_COUNT
    if expected is None:
        expected = [brute_force_oracle(case) for case in cases]
    assert len(expected) == len(cases)

    input_lines = [str(len(cases))]
    for case in cases:
        validate_case(case)
        seller_count, buyer_count, offers = case
        input_lines.append(f"{seller_count} {buyer_count}")
        input_lines.extend(
            f"{seller + 1} {buyer + 1} {goods}"
            for seller, buyer, goods in offers
        )
        input_lines.append("0 0 0")

    input_data = "\n".join(input_lines)
    if final_newline:
        input_data += "\n"
    (out_dir / f"{suite_name}.in").write_text(input_data, encoding="ascii")
    (out_dir / f"{suite_name}.out").write_text(
        "\n".join(map(str, expected)) + "\n",
        encoding="ascii",
    )


def random_case(rng: random.Random, seller_count: int, buyer_count: int) -> Case:
    offers: list[Offer] = []
    for seller in range(seller_count):
        for buyer in range(buyer_count):
            if rng.randrange(3) != 0:
                continue
            offers.append((seller, buyer, rng.randint(1, MAX_GOODS)))
    return seller_count, buyer_count, offers


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    official_cases: list[Case] = [
        (3, 2, [(0, 0, 10), (1, 0, 19), (1, 1, 11), (2, 1, 1)]),
        (
            4,
            4,
            [
                (0, 0, 6),
                (0, 1, 6),
                (1, 0, 8),
                (1, 2, 9),
                (1, 3, 8),
                (2, 1, 8),
                (3, 2, 7),
            ],
        ),
        (3, 2, [(0, 0, 10), (1, 0, 21), (1, 1, 11), (2, 1, 1)]),
    ]
    assert [brute_force_oracle(case) for case in official_cases] == [21, 29, 22]
    write_suite(out_dir, "official_sample", official_cases)

    edge_cases: list[Case] = [
        (1, 1, []),
        (1, 4, [(0, 0, 7), (0, 2, 100), (0, 3, 99)]),
        (4, 1, [(0, 0, 4), (1, 0, 80), (3, 0, 79)]),
        (3, 3, [(0, 0, 50), (0, 0, 90), (1, 1, 1), (2, 2, 2)]),
    ]
    write_suite(out_dir, "empty_unbalanced_and_duplicate_offers", edge_cases)

    rng = random.Random(133134)
    random_cases = [
        random_case(rng, rng.randint(1, 7), rng.randint(1, 7))
        for _ in range(80)
    ]
    write_suite(out_dir, "random_small_bruteforce", random_cases)

    maximum_test_cases: list[Case] = []
    maximum_test_answers: list[int] = []
    for index in range(MAX_TEST_COUNT):
        goods = index % MAX_GOODS + 1
        maximum_test_cases.append((1, 1, [(0, 0, goods)]))
        maximum_test_answers.append(goods)
    write_suite(
        out_dir,
        "maximum_test_count",
        maximum_test_cases,
        maximum_test_answers,
    )

    complete_offers = [
        (seller, buyer, MAX_GOODS if seller == buyer else 1)
        for seller in range(MAX_SELLERS)
        for buyer in range(MAX_BUYERS)
    ]
    write_suite(
        out_dir,
        "maximum_city_counts_and_offers",
        [(MAX_SELLERS, MAX_BUYERS, complete_offers)],
        [MAX_SELLERS * MAX_GOODS],
    )

    eof_case = random_case(rng, 7, 6)
    write_suite(
        out_dir,
        "without_final_newline",
        [eof_case],
        final_newline=False,
    )


if __name__ == "__main__":
    main()
