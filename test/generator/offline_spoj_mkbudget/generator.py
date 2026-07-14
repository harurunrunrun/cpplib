#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_MONTHS = 24
MAX_WORKERS = 5000

Case = tuple[int, int, int, list[int]]


def quadratic_dp_oracle(case: Case) -> int:
    """Independent quadratic-transition oracle for small cases."""
    hiring_cost, monthly_salary, severance_cost, required_workers = case
    maximum_workers = max(required_workers, default=0)
    infinity = 10**100
    previous = [infinity] * (maximum_workers + 1)
    previous[0] = 0
    for required in required_workers:
        current = [infinity] * (maximum_workers + 1)
        for workers in range(required, maximum_workers + 1):
            for old_workers, old_cost in enumerate(previous):
                if old_cost == infinity:
                    continue
                if workers >= old_workers:
                    change_cost = (workers - old_workers) * hiring_cost
                else:
                    change_cost = (old_workers - workers) * severance_cost
                current[workers] = min(
                    current[workers],
                    old_cost + change_cost + workers * monthly_salary,
                )
        previous = current
    return min(previous)


def validate_case(case: Case) -> None:
    hiring_cost, monthly_salary, severance_cost, required_workers = case
    assert 1 <= len(required_workers) <= MAX_MONTHS
    assert hiring_cost > 0
    assert monthly_salary > 0
    assert severance_cost > 0
    assert all(0 <= required <= MAX_WORKERS for required in required_workers)


def write_suite(
    out_dir: Path,
    suite_name: str,
    cases: list[Case],
    expected: list[int] | None = None,
    *,
    final_newline: bool = True,
) -> None:
    if expected is None:
        expected = [quadratic_dp_oracle(case) for case in cases]
    assert len(expected) == len(cases)

    input_lines: list[str] = []
    output_lines: list[str] = []
    for case_number, (case, answer) in enumerate(zip(cases, expected), start=1):
        validate_case(case)
        hiring_cost, monthly_salary, severance_cost, required_workers = case
        input_lines.append(
            " ".join(
                map(
                    str,
                    [
                        len(required_workers),
                        hiring_cost,
                        monthly_salary,
                        severance_cost,
                        *required_workers,
                    ],
                )
            )
        )
        output_lines.append(f"Case {case_number}, cost = ${answer}")
    input_lines.append("0")

    input_data = "\n".join(input_lines)
    if final_newline:
        input_data += "\n"
    (out_dir / f"{suite_name}.in").write_text(input_data, encoding="ascii")
    (out_dir / f"{suite_name}.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    official_cases: list[Case] = [
        (400, 500, 600, [10, 9, 11]),
        (400, 600, 600, [11, 9, 10, 14, 9, 9, 13, 15]),
    ]
    assert [quadratic_dp_oracle(case) for case in official_cases] == [19900, 66600]
    write_suite(out_dir, "official_sample", official_cases)

    edge_cases: list[Case] = [
        (1, 1, 1, [0]),
        (100, 1, 100, [10, 0, 10]),
        (2, 100, 3, [4, 1, 4, 0]),
        (7, 9, 11, [0] * MAX_MONTHS),
    ]
    write_suite(out_dir, "zero_demand_and_retention_boundaries", edge_cases)

    rng = random.Random(134133)
    random_cases: list[Case] = []
    for _ in range(60):
        month_count = rng.randint(1, 8)
        random_cases.append(
            (
                rng.randint(1, 30),
                rng.randint(1, 30),
                rng.randint(1, 30),
                [rng.randint(0, 8) for _ in range(month_count)],
            )
        )
    write_suite(out_dir, "random_small_quadratic_oracle", random_cases)

    hiring_cost = 1_000_000
    monthly_salary = 999_983
    severance_cost = 1_000_003
    constant_maximum_case: Case = (
        hiring_cost,
        monthly_salary,
        severance_cost,
        [MAX_WORKERS] * MAX_MONTHS,
    )
    constant_maximum_answer = MAX_WORKERS * (
        hiring_cost + MAX_MONTHS * monthly_salary
    )
    write_suite(
        out_dir,
        "maximum_months_workers_and_cost_scale",
        [constant_maximum_case],
        [constant_maximum_answer],
    )

    eof_case: Case = (17, 23, 19, [4, 2, 7, 1, 5])
    write_suite(
        out_dir,
        "without_final_newline",
        [eof_case],
        final_newline=False,
    )


if __name__ == "__main__":
    main()
