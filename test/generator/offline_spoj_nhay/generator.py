#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def positions_naive(text: str, pattern: str) -> list[int]:
    if not pattern:
        return list(range(len(text) + 1))
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
    input_lines: list[str] = []
    output_parts: list[str] = []
    for index, ((pattern, text), positions) in enumerate(zip(cases, answers)):
        input_lines.extend((str(len(pattern)), pattern, text))
        if index != 0:
            output_parts.append("\n")
        output_parts.extend(f"{position}\n" for position in positions)
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n", encoding="ascii"
    )
    (out_dir / f"{name}.out").write_text("".join(output_parts), encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(
        out_dir,
        "sample",
        [
            ("na", "banananobano"),
            ("foobar", "foo"),
            ("foobarfoo", "barfoobarfoobarfoobarfoobarfoo"),
        ],
    )
    write_case(
        out_dir,
        "fixed",
        [
            ("a", "a"),
            ("", "abc"),
            ("abc", ""),
            ("", ""),
            ("aa", "aaaaa"),
            ("aba", "abababa"),
            ("needle", "haystack"),
            ("abcab", "abcabcababcab"),
            ("xyz", "xyzxyzxyz"),
        ],
    )
    (out_dir / "crlf_and_eof.in").write_bytes(
        b"2\r\nna\r\nbanana\r\n3\r\nabc\r\nzzabc"
    )
    (out_dir / "crlf_and_eof.out").write_bytes(b"2\n4\n\n2\n")


    for seed in range(5):
        rng = random.Random(870870 + seed)
        cases = []
        for _ in range(25):
            pattern = "".join(rng.choice("abcde") for _ in range(rng.randint(1, 24)))
            text = "".join(rng.choice("abcde") for _ in range(rng.randint(1, 180)))
            cases.append((pattern, text))
        write_case(out_dir, f"random_{seed:02d}", cases)

    pattern = "a" * 249_999 + "b"
    text = "c" * 300_000 + pattern + "c" + pattern + "c" * 300_000
    write_case(
        out_dir,
        "large_stream",
        [(pattern, text)],
        [[300_000, 550_001]],
    )


if __name__ == "__main__":
    main()
