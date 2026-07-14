#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def positions_naive(text: str, pattern: str) -> list[int]:
    return [
        position
        for position in range(len(text) - len(pattern) + 1)
        if text[position:position + len(pattern)] == pattern
    ]


def write_case(
    out_dir: Path,
    name: str,
    cases: list[tuple[str, str]],
    answers: list[list[int]] | None = None,
) -> None:
    if answers is None:
        answers = [positions_naive(text, pattern) for pattern, text in cases]
    assert len(answers) == len(cases)
    input_lines = [str(len(cases))]
    output_lines: list[str] = []
    for (pattern, text), positions in zip(cases, answers):
        assert pattern and text
        input_lines.append(f"{pattern} {text}")
        output_lines.append(" ".join(map(str, [len(positions), *positions])))
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(output_lines) + "\n", encoding="ascii"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    fixed = [
        ("a", "a"),
        ("aa", "aaaaa"),
        ("aba", "abababa"),
        ("needle", "haystack"),
        ("foobarfoo", "barfoobarfoobarfoobarfoobarfoo"),
        ("abcab", "abcabcababcab"),
    ]
    write_case(out_dir, "fixed", fixed)

    for seed in range(5):
        rng = random.Random(87000 + seed)
        cases = []
        for _ in range(40):
            pattern = "".join(rng.choice("abcd") for _ in range(rng.randint(1, 30)))
            text = "".join(rng.choice("abcd") for _ in range(rng.randint(1, 150)))
            cases.append((pattern, text))
        write_case(out_dir, f"random_{seed:02d}", cases)

    pattern = "a" * 99_999 + "b"
    text = "c" * 200_000 + pattern + "c" * 200_000 + pattern
    write_case(out_dir, "large", [(pattern, text)], [[200_000, 500_000]])


if __name__ == "__main__":
    main()
