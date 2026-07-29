from __future__ import annotations


def first_fit_decreasing(items: list[int], capacity: int) -> int:
    remaining: list[int] = []
    for item in sorted(items, reverse=True):
        for index, available in enumerate(remaining):
            if item <= available:
                remaining[index] -= item
                break
        else:
            remaining.append(capacity - item)
    return len(remaining)
