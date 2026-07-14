#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
import string
from pathlib import Path


MAX_LENGTH = 100000


def repetition_exponent_brute(text: str) -> int:
    for exponent in range(len(text), 0, -1):
        if len(text) % exponent != 0:
            continue
        unit_length = len(text) // exponent
        if text[:unit_length] * exponent == text:
            return exponent
    raise AssertionError("non-empty string always has exponent one")


def write_case(
    out_dir: Path,
    stem: str,
    texts: list[str],
    *,
    sentinel: bool,
) -> None:
    assert all(1 <= len(text) <= MAX_LENGTH for text in texts)
    input_lines = list(texts)
    if sentinel:
        input_lines.append("*")
    (out_dir / f"{stem}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{stem}.out").write_text(
        "".join(f"{repetition_exponent_brute(text)}\n" for text in texts),
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        "official_sample",
        ["abcabcabcabc", "abcdefgh012", "aaaaaaaaaa"],
        sentinel=True,
    )
    write_case(out_dir, "sentinel_immediately", [], sentinel=True)

    rng = random.Random(20260714)
    small: list[str] = [
        "a",
        "0",
        "abab",
        "aba",
        "abcabcabc",
        "00010001",
        "zzzzzzzzzzzz",
    ]
    alphabet = string.ascii_lowercase + string.digits
    for _ in range(300):
        unit = "".join(rng.choice(alphabet) for _ in range(rng.randint(1, 14)))
        exponent = rng.randint(1, max(1, 30 // len(unit)))
        small.append(unit * exponent)
        small.append("".join(rng.choice(alphabet) for _ in range(rng.randint(1, 30))))
    write_case(out_dir, "small_bruteforce", small, sentinel=True)

    # No sentinel: the adapter must also finish cleanly on EOF.
    write_case(
        out_dir,
        "eof_termination",
        ["xyzxyzxyz", "1234512345", "notaperiod"],
        sentinel=False,
    )

    primitive_50000 = "a" * 49999 + "b"
    maximum = [
        "a" * MAX_LENGTH,
        "abcde" * 20000,
        "0123456789" * 10000,
        "a" * 99999 + "b",
        primitive_50000 * 2,
    ]
    write_case(out_dir, "maximum_length", maximum, sentinel=True)


if __name__ == "__main__":
    main()
