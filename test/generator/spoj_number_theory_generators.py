#!/usr/bin/env python3

import argparse
import functools
import math
import random
import sys
from array import array
from pathlib import Path

from spoj_number_theory_tail_generators import TAIL_GENERATORS


MOD1 = 1_000_000_007
sys.set_int_max_str_digits(0)


def write_case(out_dir, name, input_text, output_text):
    (out_dir / f"{name}.in").write_text(input_text)
    (out_dir / f"{name}.out").write_text(output_text)


def count_squarefree_brute(n):
    excluded = bytearray(n + 1)
    for base in range(2, math.isqrt(n) + 1):
        square = base * base
        excluded[square::square] = b"\1" * (n // square)
    return n - sum(excluded)


def generate_sqfree(out_dir):
    rng = random.Random(176)
    values = [1, 1000] + [rng.randrange(1, 20_000) for _ in range(24)]
    answers = [count_squarefree_brute(n) for n in values]
    values.append(100_000_000_000_000)
    answers.append(60_792_710_185_947)
    write_case(out_dir, "mixed", f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
               "\n".join(map(str, answers)) + "\n")


def gcd_extreme_brute(limit):
    return sum(math.gcd(left, right)
               for left in range(1, limit)
               for right in range(left + 1, limit + 1))


def generate_gcdex(out_dir):
    queries = [1, 2, 3, 4, 10, 100, 999, 200_000, 1_000_000]
    pinned = {
        200_000: 143_295_493_160,
        1_000_000: 4_071_628_673_912,
    }
    answers = [pinned[n] if n in pinned else gcd_extreme_brute(n)
               for n in queries]
    write_case(out_dir, "boundary", "\n".join(map(str, queries + [0])) + "\n",
               "\n".join(map(str, answers)) + "\n")


def phi_trial(n):
    result = n
    value = n
    prime = 2
    while prime * prime <= value:
        if value % prime == 0:
            result -= result // prime
            while value % prime == 0:
                value //= prime
        prime += 1 if prime == 2 else 2
    if value > 1:
        result -= result // value
    return result


def divisors(n):
    result = []
    for d in range(1, math.isqrt(n) + 1):
        if n % d == 0:
            result.append(d)
            if d * d != n:
                result.append(n // d)
    return result


def lcm_sum_oracle(n):
    s = sum(d * phi_trial(d) for d in divisors(n))
    return n * (s + 1) // 2


def generate_lcmsum(out_dir):
    values = [1, 2, 5, 6, 36, 999_983, 1_000_000]
    write_case(out_dir, "boundary", f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
               "\n".join(str(lcm_sum_oracle(n)) for n in values) + "\n")


def proper_divisor_sum_brute(n):
    if n == 1:
        return 0
    total = 1
    for d in range(2, math.isqrt(n) + 1):
        if n % d == 0:
            total += d
            if d * d != n:
                total += n // d
    return total


def generate_divsum2(out_dir):
    rng = random.Random(179)
    values = [1, 2, 10, 20] + [rng.randrange(1, 1_000_000) for _ in range(492)]
    answers = [proper_divisor_sum_brute(n) for n in values]
    large_prime = 9_999_999_967
    square_prime = 99_999_989
    other_prime = 99_999_971
    assert all(phi_trial(prime) == prime - 1
               for prime in (large_prime, square_prime, other_prime))
    values.extend([large_prime, square_prime ** 2, square_prime * other_prime])
    answers.extend([1, square_prime + 1, square_prime + other_prime + 1])
    maximum = 10 ** 16
    sigma_maximum = (2 ** 17 - 1) * (5 ** 17 - 1) // 4
    values.append(maximum)
    answers.append(sigma_maximum - maximum)
    assert len(values) == 500
    write_case(out_dir, "mixed", f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
               "\n".join(map(str, answers)) + "\n")


def generate_etf(out_dir):
    values = [1, 2, 3, 10, 36, 999_983, 1_000_000]
    write_case(out_dir, "boundary", f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
               "\n".join(str(phi_trial(n)) for n in values) + "\n")


def matrix_multiply(a, b, modulus=MOD1):
    n = len(a)
    result = [[0] * n for _ in range(n)]
    for i in range(n):
        for k in range(n):
            for j in range(n):
                result[i][j] = (result[i][j] + a[i][k] * b[k][j]) % modulus
    return result


def matrix_power(a, exponent, modulus=MOD1):
    n = len(a)
    result = [[int(i == j) for j in range(n)] for i in range(n)]
    while exponent:
        if exponent & 1:
            result = matrix_multiply(result, a, modulus)
        a = matrix_multiply(a, a, modulus)
        exponent //= 2
    return result


def generate_mpow(out_dir):
    cases = [([[1, 0], [1, 1]], 3), ([[1, 0, 4], [1, 2, 2], [0, 4, 4]], 3)]
    identity = [[int(i == j) for j in range(50)] for i in range(50)]
    cases.append((identity, 100_000))
    rng = random.Random(181)
    dense = [[rng.randrange(1_000_000_001) for _ in range(50)]
             for _ in range(50)]
    dense[0][0] = 0
    dense[0][1] = 1_000_000_000
    cases.append((dense, 100_000))
    input_lines = [str(len(cases))]
    output_lines = []
    for matrix, exponent in cases:
        input_lines.append(f"{len(matrix)} {exponent}")
        input_lines.extend(" ".join(map(str, row)) for row in matrix)
        output_lines.extend(" ".join(map(str, row)) for row in matrix_power(matrix, exponent))
    write_case(out_dir, "boundary", "\n".join(input_lines) + "\n", "\n".join(output_lines) + "\n")


def recurrence_brute(initial, coefficient, index, modulus=1_000_000_000):
    sequence = list(initial)
    while len(sequence) < index:
        value = sum(coefficient[j] * sequence[-1 - j] for j in range(len(coefficient)))
        sequence.append(value % modulus)
    return sequence[index - 1] % modulus


def recurrence_companion_oracle(initial, coefficient, index,
                                modulus=1_000_000_000):
    degree = len(initial)
    if index <= degree:
        return initial[index - 1] % modulus

    transition = [[0] * degree for _ in range(degree)]
    transition[0] = [value % modulus for value in coefficient]
    for row in range(1, degree):
        transition[row][row - 1] = 1
    powered = matrix_power(transition, index - degree, modulus)
    state = list(reversed(initial))
    return sum(powered[0][column] * (state[column] % modulus)
               for column in range(degree)) % modulus


def generate_seq(out_dir):
    rng = random.Random(183)
    cases = [([5, 8, 2], [32, 54, 6], 2), ([1, 2, 3], [4, 5, 6], 6)]
    for _ in range(20):
        degree = rng.randrange(1, 8)
        cases.append(([rng.randrange(1000) for _ in range(degree)],
                      [rng.randrange(1000) for _ in range(degree)], rng.randrange(1, 50)))
    cases.append(([987_654_321], [1], 1_000_000_000))
    degree = 10
    cases.append((
        [999_999_999 - 7_919 * index for index in range(degree)],
        [999_999_937 - 65_537 * index for index in range(degree)],
        1_000_000_000,
    ))
    lines = [str(len(cases))]
    answers = []
    for initial, coefficient, index in cases:
        lines.extend([str(len(initial)), " ".join(map(str, initial)),
                      " ".join(map(str, coefficient)), str(index)])
        answer = recurrence_companion_oracle(initial, coefficient, index)
        if index <= 50:
            assert answer == recurrence_brute(initial, coefficient, index)
        answers.append(answer)
    write_case(out_dir, "mixed", "\n".join(lines) + "\n", "\n".join(map(str, answers)) + "\n")


def affine_oracle(a, b, n, modulus):
    result_mul, result_add = 1 % modulus, 0
    base_mul, base_add = a % modulus, b % modulus
    while n:
        if n & 1:
            result_mul, result_add = ((result_mul * base_mul) % modulus,
                                      (result_mul * base_add + result_add) % modulus)
        base_mul, base_add = ((base_mul * base_mul) % modulus,
                              (base_mul * base_add + base_add) % modulus)
        n //= 2
    return (result_mul + result_add) % modulus


def generate_rec(out_dir):
    cases = [(1, 1, 1, 10), (2, 1, 2, 5), (5, 2, 20, 30),
             (10 ** 100, 10 ** 100 - 1, 10 ** 100 - 7, 100_000)]
    lines = [str(len(cases))] + [" ".join(map(str, case)) for case in cases]
    answers = [affine_oracle(*case) for case in cases]
    write_case(out_dir, "boundary", "\n".join(lines) + "\n", "\n".join(map(str, answers)) + "\n")


def fibonacci_pair(n, modulus=MOD1):
    if n == 0:
        return 0, 1
    a, b = fibonacci_pair(n // 2, modulus)
    c = a * ((2 * b - a) % modulus) % modulus
    d = (a * a + b * b) % modulus
    return (d, (c + d) % modulus) if n & 1 else (c, d)


def generate_fibosum(out_dir):
    ranges = [(0, 3), (3, 5), (10, 19), (0, 1_000_000_000),
              (999_999_999, 1_000_000_000)]
    answers = [(fibonacci_pair(right + 2)[0] - fibonacci_pair(left + 1)[0]) % MOD1
               for left, right in ranges]
    write_case(out_dir, "boundary", f"{len(ranges)}\n" +
               "\n".join(f"{a} {b}" for a, b in ranges) + "\n",
               "\n".join(map(str, answers)) + "\n")


def is_lucifer_number_brute(value):
    difference = sum((1 if position % 2 == 0 else -1) * int(digit)
                     for position, digit in enumerate(reversed(str(value)), 1))
    return difference >= 2 and all(
        difference % divisor for divisor in range(2, math.isqrt(difference) + 1)
    )


def lucifer_count_brute(left, right):
    return sum(is_lucifer_number_brute(value)
               for value in range(left, right + 1))


def generate_lucifer(out_dir):
    ranges = [(200, 250), (150, 200), (100, 150), (50, 100), (0, 50),
              (0, 5000), (4321, 9876),
              (0, 1_000_000_000), (999_999_000, 1_000_000_000)]
    pinned = {
        (0, 1_000_000_000): 130_629_858,
        (999_999_000, 1_000_000_000): 85,
    }
    answers = [pinned[(left, right)] if (left, right) in pinned
               else lucifer_count_brute(left, right)
               for left, right in ranges]
    write_case(out_dir, "boundary", f"{len(ranges)}\n" +
               "\n".join(f"{a} {b}" for a, b in ranges) + "\n",
               "\n".join(map(str, answers)) + "\n")


def generate_gcd2(out_dir):
    cases = [(2, "6"), (10, "11"), (0, "000123456789"),
             (40_000, "9" * 250), (39_999, "1" + "0" * 249)]
    answers = [str(math.gcd(a, int(b))) for a, b in cases]
    write_case(out_dir, "boundary", f"{len(cases)}\n" +
               "\n".join(f"{a} {b}" for a, b in cases) + "\n",
               "\n".join(answers) + "\n")


def fraction_tree_stern(index):
    memo = {0: 0, 1: 1}

    def stern(value):
        if value not in memo:
            half = value // 2
            memo[value] = (stern(half) if value % 2 == 0
                           else stern(half) + stern(half + 1))
        return memo[value]

    return stern(index), stern(index + 1)


def generate_ng0frctn(out_dir):
    values = [1, 2, 3, 7, 12345, 10_000_000_000]
    answers = ["%d/%d" % fraction_tree_stern(n) for n in values]
    write_case(out_dir, "boundary", "\n".join(map(str, values + [0])) + "\n",
               "\n".join(answers) + "\n")


def factor_trial(n):
    result = []
    divisor = 2
    while divisor * divisor <= n:
        if n % divisor == 0:
            exponent = 0
            while n % divisor == 0:
                n //= divisor
                exponent += 1
            result.append((divisor, exponent))
        divisor += 1 if divisor == 2 else 2
    if n > 1:
        result.append((n, 1))
    return result


def generate_fact0(out_dir):
    maximum_case = 999_983 * 999_983 * 997
    values = [1, 3_111_989, 13_091_989, 77_145_199_750_673, maximum_case]
    known = {
        77_145_199_750_673: [(328_439, 1), (234_884_407, 1)],
        maximum_case: [(997, 1), (999_983, 2)],
    }
    answers = []
    for value in values:
        factors = known[value] if value in known else factor_trial(value)
        answers.append(" ".join(f"{p}^{e}" for p, e in factors))
    write_case(out_dir, "boundary", "\n".join(map(str, values + [0])) + "\n",
               "\n".join(answers) + "\n")


def sampled_primes_oracle(limit=100_000_000):
    odd_count = limit // 2
    sieve = bytearray(b"\1") * odd_count
    sieve[0] = 0
    for prime in range(3, math.isqrt(limit - 1) + 1, 2):
        if sieve[prime // 2]:
            start = prime * prime // 2
            count = (odd_count - start - 1) // prime + 1
            sieve[start::prime] = b"\0" * count
    result = []
    index = 1
    result.append(2)
    for half in range(1, odd_count):
        if sieve[half]:
            index += 1
            if (index - 1) % 100 == 0:
                result.append(2 * half + 1)
    return result


def generate_tdprimes(out_dir):
    write_case(out_dir, "full", "", "\n".join(map(str, sampled_primes_oracle())) + "\n")


def lucky_values(rank):
    values = []
    candidate = 2
    while len(values) < rank:
        if len(factor_trial(candidate)) >= 3:
            values.append(candidate)
        candidate += 1
    return values


def generate_amr11e(out_dir):
    ranks = [1, 2, 3, 10, 100, 999, 1000]
    values = lucky_values(max(ranks))
    write_case(out_dir, "boundary", f"{len(ranks)}\n" + "\n".join(map(str, ranks)) + "\n",
               "\n".join(str(values[r - 1]) for r in ranks) + "\n")


def smallest_prime_factor_trial(value):
    for divisor in range(2, math.isqrt(value) + 1):
        if value % divisor == 0:
            return divisor
    return value


def spf_prefix_values_trial(limit):
    prefix = [0] * (limit + 1)
    for value in range(2, limit + 1):
        prefix[value] = prefix[value - 1] + smallest_prime_factor_trial(value)
    return prefix


def generate_aps(out_dir):
    values = [2, 3, 4, 10, 100, 999, 1000, 9_999_999]
    prefix = spf_prefix_values_trial(1000)
    pinned = {9_999_999: 3_203_714_961_607}
    answers = {value: (pinned[value] if value in pinned else prefix[value])
               for value in values}
    write_case(out_dir, "boundary", f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
               "\n".join(str(answers[n]) for n in values) + "\n")


def format_exact_tower(base, height):
    if base == 0:
        return str(1 if height % 2 == 0 else 0)
    if base == 1:
        return "1"
    value = 1
    for _ in range(height):
        value = base ** value
        if value.bit_length() > 100_000:
            break
    if value < 1_000_000_000:
        return str(value)
    return "..." + str(value % 1_000_000_000).zfill(9)


def generate_powtow(out_dir):
    cases = [(0, 0), (0, 1), (0, 2), (0, 27), (0, 28), (0, 29),
             (1, 2_147_483_647), (2, 5), (3, 3),
             (993_306_745, 75_707_320)]
    answers = [
        format_exact_tower(a, h) if h <= 5 or a <= 1 else None
        for a, h in cases
    ]
    answers[-2] = format_exact_tower(3, 3)
    answers[-1] = "...884765625"
    write_case(out_dir, "boundary", f"{len(cases)}\n" +
               "\n".join(f"{a} {h}" for a, h in cases) + "\n",
               "\n".join(answers) + "\n")


def pisano_brute(modulus):
    previous, current = 0, 1
    for period in range(1, 6 * modulus + 1):
        previous, current = current, (previous + current) % modulus
        if previous == 0 and current == 1:
            return period
    raise AssertionError("period not found")


def generate_pisano(out_dir):
    values = [2, 3, 4, 5, 6, 7, 10, 97]
    answers = [pisano_brute(n) for n in values]
    values.append(10 ** 12)
    answers.append(math.lcm(3 * 2 ** 11, 20 * 5 ** 11))
    write_case(out_dir, "boundary", f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
               "\n".join(map(str, answers)) + "\n")


def generate_prlcm(out_dir):
    rng = random.Random(198)
    values = [rng.randrange(1, 100) for _ in range(80)]
    answer = sum(math.lcm(values[i], values[j]) for i in range(len(values))
                 for j in range(i + 1, len(values))) % 998_244_353
    write_case(out_dir, "random", f"{len(values)}\n" + " ".join(map(str, values)) + "\n",
               f"{answer}\n")
    size = 200_000
    maximum = 1_000_000
    answer = (size * (size - 1) // 2 * maximum) % 998_244_353
    write_case(out_dir, "boundary", f"{size}\n" + ((str(maximum) + " ") * (size - 1)) +
               str(maximum) + "\n", f"{answer}\n")


def mobius_sieve(limit):
    mobius = array("b", [0]) * (limit + 1)
    composite = bytearray(limit + 1)
    primes = []
    mobius[1] = 1
    for value in range(2, limit + 1):
        if not composite[value]:
            primes.append(value)
            mobius[value] = -1
        for prime in primes:
            product = value * prime
            if product > limit:
                break
            composite[product] = 1
            if value % prime == 0:
                mobius[product] = 0
                break
            mobius[product] = -mobius[value]
    return mobius


def visible_lattice_oracle(n, mobius):
    pairs = sum(mobius[d] * (n // d) ** 2 for d in range(1, n + 1))
    triples = sum(mobius[d] * (n // d) ** 3 for d in range(1, n + 1))
    return 3 + 3 * pairs + triples


def generate_vlattice(out_dir):
    values = [1, 2, 5, 100, 1_000_000]
    mobius = mobius_sieve(max(values))
    answers = [visible_lattice_oracle(n, mobius) for n in values]
    write_case(out_dir, "boundary", f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
               "\n".join(map(str, answers)) + "\n")


GENERATORS = {
    "sqfree": generate_sqfree,
    "gcdex": generate_gcdex,
    "lcmsum": generate_lcmsum,
    "divsum2": generate_divsum2,
    "etf": generate_etf,
    "mpow": generate_mpow,
    "seq": generate_seq,
    "rec": generate_rec,
    "fibosum": generate_fibosum,
    "lucifer": generate_lucifer,
    "gcd2": generate_gcd2,
    "ng0frctn": generate_ng0frctn,
    "fact0": generate_fact0,
    "tdprimes": generate_tdprimes,
    "amr11e": generate_amr11e,
    "aps": generate_aps,
    "powtow": generate_powtow,
    "pisano": generate_pisano,
    "prlcm": generate_prlcm,
    "vlattice": generate_vlattice,
}

for code, generator in TAIL_GENERATORS.items():
    GENERATORS[code] = functools.partial(generator, write_case=write_case)


def main(code):
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    GENERATORS[code](out_dir)
