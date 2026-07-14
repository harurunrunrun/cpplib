#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


MODULUS = 1_000_000_007


def multiply(lhs, rhs):
    size = len(lhs)
    result = [[0] * size for _ in range(size)]
    for row in range(size):
        for middle in range(size):
            coefficient = lhs[row][middle]
            if coefficient == 0:
                continue
            for column in range(size):
                result[row][column] = (
                    result[row][column] + coefficient * rhs[middle][column]
                ) % MODULUS
    return result


def power(matrix, exponent):
    size = len(matrix)
    result = [[int(row == column) for column in range(size)] for row in range(size)]
    base = [[value % MODULUS for value in row] for row in matrix]
    while exponent:
        if exponent & 1:
            result = multiply(result, base)
        exponent >>= 1
        if exponent:
            base = multiply(base, base)
    return result


def render_input(matrix, queries):
    lines = [str(len(matrix))]
    lines.extend(" ".join(map(str, row)) for row in matrix)
    lines.extend(f"{row} {column} {exponent}" for row, column, exponent in queries)
    return "\n".join(lines) + "\n"


def write_case(output_dir, name, matrix, queries, answers):
    (output_dir / f"{name}.in").write_text(render_input(matrix, queries))
    (output_dir / f"{name}.out").write_text("\n".join(map(str, answers)) + "\n")


def full_matrix_answers(matrix, queries):
    cache = {}
    answers = []
    for row, column, exponent in queries:
        if exponent not in cache:
            cache[exponent] = power(matrix, exponent)
        answers.append(cache[exponent][row - 1][column - 1])
    return answers


def generate_sample(output_dir):
    matrix = [
        [32, 82, 7, 66, 30],
        [57, 1, 28, 2, 89],
        [53, 66, 6, 35, 61],
        [45, 87, 88, 24, 20],
        [35, 22, 23, 80, 93],
    ]
    queries = [(1, 1, 1), (1, 5, 2), (5, 1, 3), (5, 5, 99)]
    write_case(output_dir, "sample", matrix, queries, full_matrix_answers(matrix, queries))


def generate_api_edges(output_dir):
    matrix = [[-1, 2, 0], [3, -4, 5], [0, 6, -7]]
    exponents = [0, 1, 2, 3, 31, 32, 33, 10**18]
    queries = [
        (row, column, exponent)
        for exponent in exponents
        for row in range(1, 4)
        for column in range(1, 4)
    ]
    write_case(
        output_dir,
        "api_edges",
        matrix,
        queries,
        full_matrix_answers(matrix, queries),
    )


def generate_random_dense(output_dir):
    rng = random.Random(13929)
    size = 18
    matrix = [[rng.randrange(1, 1_000_000_001) for _ in range(size)] for _ in range(size)]
    exponents = [1, 2, 3, 63, 64, 65, 999_999_937, 10**18]
    queries = []
    for exponent in exponents:
        queries.extend((rng.randrange(1, size + 1), rng.randrange(1, size + 1), exponent)
                       for _ in range(36))
    write_case(
        output_dir,
        "random_dense",
        matrix,
        queries,
        full_matrix_answers(matrix, queries),
    )


def generate_all_ones(output_dir):
    rng = random.Random(182)
    size = 18
    matrix = [[1] * size for _ in range(size)]
    queries = []
    answers = []
    boundary_exponents = [1, 2, 3, (1 << 63) - 1, 10**18]
    for query_index in range(8192):
        exponent = (boundary_exponents[query_index]
                    if query_index < len(boundary_exponents)
                    else rng.randrange(1, 10**18 + 1))
        queries.append((rng.randrange(1, size + 1), rng.randrange(1, size + 1), exponent))
        answers.append(pow(size, exponent - 1, MODULUS))
    write_case(output_dir, "dense_many_queries", matrix, queries, answers)


def generate_identity(output_dir):
    rng = random.Random(838383)
    size = 18
    matrix = [[int(row == column) for column in range(size)] for row in range(size)]
    queries = []
    answers = []
    for query_index in range(65536):
        row = rng.randrange(1, size + 1)
        column = rng.randrange(1, size + 1)
        exponent = 10**18 if query_index == 0 else rng.randrange(1, 10**18 + 1)
        queries.append((row, column, exponent))
        answers.append(int(row == column))
    write_case(output_dir, "identity_many_queries", matrix, queries, answers)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    output_dir = Path(parser.parse_args().out_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    generate_sample(output_dir)
    generate_api_edges(output_dir)
    generate_random_dense(output_dir)
    generate_all_ones(output_dir)
    generate_identity(output_dir)


if __name__ == "__main__":
    main()
