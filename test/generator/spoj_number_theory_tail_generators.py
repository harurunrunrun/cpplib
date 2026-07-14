#!/usr/bin/env python3

import math
import random


MOD1 = 1_000_000_007
ASSIEVE_MOD = 998_244_353


def fibonacci_pair(index, modulus=MOD1):
    if index == 0:
        return 0, 1
    value, next_value = fibonacci_pair(index // 2, modulus)
    doubled = value * ((2 * next_value - value) % modulus) % modulus
    advanced = (value * value + next_value * next_value) % modulus
    return (advanced, (doubled + advanced) % modulus) if index & 1 else (doubled, advanced)


def nearest_phi_power(exponent):
    if exponent == 0:
        return 1
    if exponent == 1:
        return 2
    value, next_value = fibonacci_pair(exponent)
    return (2 * next_value - value) % MOD1


def generate_powerphi(out_dir, write_case):
    rng = random.Random(197)
    values = [0, 1, 2, 3, 4, 10, 1_000_000_000]
    values += [rng.randrange(0, 1_000_000_001) for _ in range(40)]
    answers = [nearest_phi_power(value) for value in values]
    write_case(
        out_dir,
        "boundary",
        f"{len(values)}\n" + "\n".join(map(str, values)) + "\n",
        "\n".join(map(str, answers)) + "\n",
    )


def enumerate_primes(limit):
    prime = bytearray(b"\1") * (limit + 1)
    prime[0] = 0
    if limit >= 1:
        prime[1] = 0
    for value in range(2, math.isqrt(limit) + 1):
        if not prime[value]:
            continue
        start = value * value
        prime[start::value] = b"\0" * ((limit - start) // value + 1)
    return [value for value in range(2, limit + 1) if prime[value]]


def assieve_brute(n, exponent, operation):
    prefix = [[0] * (n + 1) for _ in range(4)]
    for value in range(1, n + 1):
        remaining = value
        state = 0
        prime = 2
        while prime * prime <= remaining:
            if remaining % prime == 0:
                multiplicity = 0
                while remaining % prime == 0:
                    remaining //= prime
                    multiplicity += 1
                state = operation[state][prime * multiplicity % 4]
            prime += 1 if prime == 2 else 2
        if remaining > 1:
            state = operation[state][remaining % 4]
        weight = pow(value, exponent, ASSIEVE_MOD)
        for result_state in range(4):
            prefix[result_state][value] = prefix[result_state][value - 1]
        prefix[state][value] = (prefix[state][value] + weight) % ASSIEVE_MOD
    return [
        tuple(prefix[state][n // divisor] for state in range(4))
        for divisor in range(1, math.isqrt(n) + 1)
    ]


def assieve_zero_exponent(n, operation):
    values = []
    left = 1
    while left <= n:
        quotient = n // left
        values.append(quotient)
        left = n // quotient + 1
    index_of = {value: index for index, value in enumerate(values)}
    primes = enumerate_primes(math.isqrt(n))

    odd_candidates = []
    for value in values:
        residue_one = (value - 1) // 4 + 1
        residue_three = 0 if value < 3 else (value - 3) // 4 + 1
        odd_candidates.append([(residue_one - 1) % ASSIEVE_MOD, residue_three])
    primes_before = [0, 0]
    for prime in primes:
        if prime == 2:
            continue
        prime_class = 0 if prime % 4 == 1 else 1
        square = prime * prime
        for index, value in enumerate(values):
            if value < square:
                break
            source = odd_candidates[index_of[value // prime]]
            for source_class in range(2):
                target_class = source_class ^ prime_class
                odd_candidates[index][target_class] = (
                    odd_candidates[index][target_class]
                    - source[source_class]
                    + primes_before[source_class]
                ) % ASSIEVE_MOD
        primes_before[prime_class] += 1

    prime_prefix = [
        [0, residue_one, int(value >= 2), residue_three]
        for value, (residue_one, residue_three) in zip(values, odd_candidates)
    ]
    result = [row.copy() for row in prime_prefix]
    for prime in reversed(primes):
        prime_through_current = prime_prefix[index_of[prime]]
        square = prime * prime
        for index, value in enumerate(values):
            if value < square:
                break
            power = prime
            power_exponent = 1
            while power <= value // prime:
                source = result[index_of[value // power]]
                state = prime % 4 * (power_exponent % 4) % 4
                for source_state in range(4):
                    target_state = operation[state][source_state]
                    result[index][target_state] = (
                        result[index][target_state]
                        + source[source_state]
                        - prime_through_current[source_state]
                    ) % ASSIEVE_MOD
                pure_state = prime % 4 * ((power_exponent + 1) % 4) % 4
                result[index][pure_state] = (result[index][pure_state] + 1) % ASSIEVE_MOD
                power *= prime
                power_exponent += 1
    for row in result:
        row[0] = (row[0] + 1) % ASSIEVE_MOD
    return [tuple(row) for row in result[:math.isqrt(n)]]


def format_assieve_case(n, exponent, operation, answers):
    input_text = f"{n} {exponent}\n" + "\n".join(
        " ".join(map(str, row)) for row in operation
    ) + "\n"
    output_text = "\n".join(" ".join(map(str, row)) for row in answers) + "\n"
    return input_text, output_text


def generate_assieve(out_dir, write_case):
    addition = [[(left + right) % 4 for right in range(4)] for left in range(4)]
    assert assieve_brute(200, 0, addition) == assieve_zero_exponent(200, addition)

    input_text, output_text = format_assieve_case(
        10, 0, addition, assieve_brute(10, 0, addition)
    )
    write_case(out_dir, "official_first", input_text, output_text)

    xor_operation = [[left ^ right for right in range(4)] for left in range(4)]
    official_second = [
        (457599333, 476580683, 403589597, 762762658),
        (361221912, 612412943, 661908092, 483645330),
        (242804711, 682542199, 535167020, 465246643),
        (913280460, 516845083, 917292729, 390364642),
        (39265044, 919790719, 181416471, 421087779),
        (530140662, 31014314, 181416471, 226287885),
        (982924733, 31014314, 851084249, 226287885),
        (982924733, 938693280, 851084249, 226287885),
        (982924733, 938693280, 851084249, 435036575),
        (982924733, 938693280, 851084249, 138976409),
    ]
    input_text, output_text = format_assieve_case(
        100, 100, xor_operation, official_second
    )
    write_case(out_dir, "official_second", input_text, output_text)

    maximum_exponent_n = 200
    input_text, output_text = format_assieve_case(
        maximum_exponent_n,
        1000,
        xor_operation,
        assieve_brute(maximum_exponent_n, 1000, xor_operation),
    )
    write_case(out_dir, "maximum_exponent", input_text, output_text)

    maximum_n = 10_000_000_000
    input_text, output_text = format_assieve_case(
        maximum_n,
        0,
        addition,
        assieve_zero_exponent(maximum_n, addition),
    )
    write_case(out_dir, "maximum_n", input_text, output_text)


TAIL_GENERATORS = {
    "powerphi": generate_powerphi,
    "assieve": generate_assieve,
}
