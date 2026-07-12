#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encode(value: str) -> str:
    return value if value else "-"


def count_matches(patterns: list[str], text: str) -> int:
    result = 0
    for pattern in patterns:
        if not pattern:
            result += len(text) + 1
        else:
            result += sum(
                text[index : index + len(pattern)] == pattern
                for index in range(len(text) - len(pattern) + 1)
            )
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    patterns = ["", "a", "aa", "aba", "a"]
    for _ in range(195):
        length = rng.randrange(11)
        patterns.append("".join(rng.choice("abc") for _ in range(length)))

    texts = ["", "a", "aaaa", "abacaba", "cccccc"]
    for _ in range(400):
        length = rng.randrange(81)
        texts.append("".join(rng.choice("abc") for _ in range(length)))

    lines = [f"{len(patterns)} {len(texts)}"]
    lines.extend(encode(pattern) for pattern in patterns)
    lines.extend(encode(text) for text in texts)
    outputs = [str(count_matches(patterns, text)) for text in texts]
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
