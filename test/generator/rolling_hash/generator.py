#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def encode(value: str) -> str:
    return value if value else "-"


def lcp(a: str, b: str, i: int, j: int) -> int:
    length = 0
    while i + length < len(a) and j + length < len(b) and a[i + length] == b[j + length]:
        length += 1
    return length


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    pairs = [
        ("", ""),
        ("a", "a"),
        ("abracadabra", "cadabracad"),
        ("a" * 80, "a" * 73),
        ("abcabcabc", "bcabcabca"),
    ]
    for _ in range(75):
        n = rng.randrange(81)
        m = rng.randrange(81)
        pairs.append(
            (
                "".join(rng.choice("abcde") for _ in range(n)),
                "".join(rng.choice("abcde") for _ in range(m)),
            )
        )

    input_lines = [str(len(pairs))]
    output_lines: list[str] = []
    for s, t in pairs:
        operations: list[tuple[int, ...]] = []
        for i in sorted({0, len(s) // 2, len(s)}):
            for j in sorted({0, len(t) // 2, len(t)}):
                operations.append((1, i, j))
        for _ in range(35):
            length = rng.randrange(len(s) + 1)
            l = rng.randrange(len(s) - length + 1)
            x = rng.randrange(len(s) - length + 1)
            operations.append((0, l, l + length, x))

            l = rng.randrange(len(s) + 1)
            r = rng.randrange(l, len(s) + 1)
            x = rng.randrange(len(t) + 1)
            y = rng.randrange(x, len(t) + 1)
            operations.append((2, l, r, x, y))

        input_lines.append(f"{encode(s)} {encode(t)} {len(operations)}")
        for operation in operations:
            input_lines.append(" ".join(map(str, operation)))
            if operation[0] == 0:
                _, l, r, x = operation
                output_lines.append(str(int(s[l:r] == s[x : x + r - l])))
            elif operation[0] == 1:
                _, i, j = operation
                output_lines.append(str(lcp(s, t, i, j)))
            else:
                _, l, r, x, y = operation
                output_lines.append(str(int(s[l:r] == t[x:y])))

    (out_dir / "case_00.in").write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(output_lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
