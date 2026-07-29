from __future__ import annotations


def best_move(rewards: list[int]) -> int:
    return max(range(len(rewards)), key=rewards.__getitem__)
