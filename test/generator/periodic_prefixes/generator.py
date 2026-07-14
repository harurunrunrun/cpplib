#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def periodic_prefixes_brute(text: str) -> list[tuple[int, int]]:
    result = []
    for prefix_length in range(2, len(text) + 1):
        prefix = text[:prefix_length]
        for block_length in range(1, prefix_length):
            if prefix_length % block_length != 0:
                continue
            repetition_count = prefix_length // block_length
            if prefix == prefix[:block_length] * repetition_count:
                result.append((prefix_length, repetition_count))
                break
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    texts = [
        "aa",
        "ab",
        "aaa",
        "ababab",
        "aabaabaabaab",
        "abcabcabcabcx",
        "{~{~",
    ]
    source = random.Random(25030088)
    for _ in range(200):
        texts.append("".join(
            source.choice("abc{~")
            for _ in range(source.randint(1, 32))
        ))

    input_lines = [str(len(texts)), *texts]
    output_lines: list[str] = []
    for text in texts:
        result = periodic_prefixes_brute(text)
        output_lines.append(str(len(result)))
        output_lines.extend(f"{length} {count}" for length, count in result)

    (out_dir / "case_00.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(output_lines) + "\n",
        encoding="ascii",
    )


if __name__ == "__main__":
    main()
