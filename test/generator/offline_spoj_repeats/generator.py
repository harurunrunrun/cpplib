#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def solve_naive(text: str) -> int:
    answer = 1
    for left in range(len(text)):
        for period in range(1, len(text) - left + 1):
            seed = text[left:left + period]
            count = 1
            while text[left + count * period:left + (count + 1) * period] == seed:
                count += 1
            answer = max(answer, count)
    return answer


def write_case(out_dir: Path, name: str, cases: list[tuple[str, int]]) -> None:
    input_lines = [str(len(cases))]
    for text, _ in cases:
        input_lines.append(str(len(text)))
        input_lines.extend(text)
    (out_dir / f"{name}.in").write_text(
        "\n".join(input_lines) + "\n",
        encoding="ascii",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(str(answer) for _, answer in cases) + "\n",
        encoding="ascii",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    out_dir = parser.parse_args().out_dir
    out_dir.mkdir(parents=True, exist_ok=True)

    sample = "babbabaabaabaabab"
    fixed = ["a", "aaaaa", "abababab", "xababab", sample, "abcdef"]
    write_case(out_dir, "fixed", [(text, solve_naive(text)) for text in fixed])

    for seed in range(4):
        rng = random.Random(82080 + seed)
        texts = [
            "".join(rng.choice("ab") for _ in range(rng.randint(1, 30)))
            for _ in range(25)
        ]
        write_case(
            out_dir,
            f"random_{seed:02d}",
            [(text, solve_naive(text)) for text in texts],
        )

    size = 50_000
    boundary: list[tuple[str, int]] = []
    for index in range(20):
        if index % 3 == 0:
            boundary.append(("a" * size, size))
        elif index % 3 == 1:
            boundary.append(("ab" * (size // 2), size // 2))
        else:
            boundary.append(("a" + "b" * (size - 1), size - 1))
    write_case(out_dir, "maximum_constraints", boundary)


if __name__ == "__main__":
    main()
