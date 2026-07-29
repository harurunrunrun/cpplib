from __future__ import annotations


def solve_diagonal(diagonal: list[int], right_hand_side: list[int]) -> list[int]:
    if len(diagonal) != len(right_hand_side):
        raise ValueError("dimension mismatch")
    result: list[int] = []
    for coefficient, value in zip(diagonal, right_hand_side):
        if coefficient <= 0 or value % coefficient != 0:
            raise ValueError("the generated diagonal system must have an integer solution")
        result.append(value // coefficient)
    return result
