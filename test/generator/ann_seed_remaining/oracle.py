#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations


def solve_case(text: str, pattern: str, maximum_mismatches: int) -> str:
    matches: list[tuple[int, int]] = []
    for position in range(len(text) - len(pattern) + 1):
        mismatches = sum(
            left != right
            for left, right in zip(
                text[position : position + len(pattern)],
                pattern,
            )
        )
        if mismatches <= maximum_mismatches:
            matches.append((position, mismatches))
    fields = [str(len(matches))]
    for position, mismatches in matches:
        fields.extend((str(position), str(mismatches)))
    return " ".join(fields)
