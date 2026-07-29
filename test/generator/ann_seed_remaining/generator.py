#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path

from oracle import solve_case


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260729)
    cases: list[tuple[str, str, int, int]] = [
        ("ACGTACGA", "ACGA", 1, 0),
        ("AAAA", "TT", 2, 2),
        ("GATTACAGATTACA", "GACTACA", 2, 1),
        ("ACGT", "ACGT", 4, 0),
        ("AAAAAA", "AAA", 2, 0),
        ("ACACAC", "GTG", 1, 1),
    ]
    alphabet = "ACGT"
    for _ in range(120):
        text_length = rng.randrange(1, 90)
        pattern_length = rng.randrange(1, text_length + 1)
        text = "".join(
            rng.choice(alphabet) for _ in range(text_length)
        )
        if rng.randrange(3) == 0:
            position = rng.randrange(text_length - pattern_length + 1)
            pattern = list(text[position : position + pattern_length])
            for _ in range(rng.randrange(0, min(5, pattern_length) + 1)):
                changed = rng.randrange(pattern_length)
                pattern[changed] = rng.choice(alphabet)
            pattern_text = "".join(pattern)
        else:
            pattern_text = "".join(
                rng.choice(alphabet)
                for _ in range(pattern_length)
            )
        seed_length = rng.randrange(1, pattern_length + 1)
        maximum_mismatches = rng.randrange(0, pattern_length + 1)
        cases.append(
            (
                text,
                pattern_text,
                seed_length,
                maximum_mismatches,
            )
        )

    for case_index in range(6):
        selected = cases[case_index::6]
        inputs = [str(len(selected))]
        outputs: list[str] = []
        for text, pattern, seed_length, maximum_mismatches in selected:
            inputs.append(
                f"{text} {pattern} {seed_length} {maximum_mismatches}"
            )
            outputs.append(
                solve_case(text, pattern, maximum_mismatches)
            )
        stem = f"case_{case_index:02d}"
        (out_dir / f"{stem}.in").write_text(
            "\n".join(inputs) + "\n",
            encoding="utf-8",
        )
        (out_dir / f"{stem}.out").write_text(
            "\n".join(outputs) + "\n",
            encoding="utf-8",
        )


if __name__ == "__main__":
    main()
