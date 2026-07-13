#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


Case = tuple[list[int], list[int], list[int], tuple[int, list[int], list[int]] | None]


def traversals(
    root: int,
    left: list[int],
    right: list[int],
    keys: list[int],
) -> tuple[list[int], list[int], list[int]]:
    preorder: list[int] = []
    stack = [root] if root != -1 else []
    while stack:
        vertex = stack.pop()
        preorder.append(keys[vertex])
        if right[vertex] != -1:
            stack.append(right[vertex])
        if left[vertex] != -1:
            stack.append(left[vertex])

    inorder: list[int] = []
    stack = []
    vertex = root
    while vertex != -1 or stack:
        while vertex != -1:
            stack.append(vertex)
            vertex = left[vertex]
        vertex = stack.pop()
        inorder.append(keys[vertex])
        vertex = right[vertex]

    postorder: list[int] = []
    expanded_stack = [(root, False)] if root != -1 else []
    while expanded_stack:
        vertex, expanded = expanded_stack.pop()
        if expanded:
            postorder.append(keys[vertex])
            continue
        expanded_stack.append((vertex, True))
        if right[vertex] != -1:
            expanded_stack.append((right[vertex], False))
        if left[vertex] != -1:
            expanded_stack.append((left[vertex], False))
    return preorder, inorder, postorder


def expected_tree(
    root: int,
    left: list[int],
    right: list[int],
    keys: list[int],
    inorder: list[int],
) -> tuple[int, list[int], list[int]]:
    position = {key: index for index, key in enumerate(inorder)}
    result_left = [-1] * len(keys)
    result_right = [-1] * len(keys)
    for vertex in range(len(keys)):
        at = position[keys[vertex]]
        if left[vertex] != -1:
            result_left[at] = position[keys[left[vertex]]]
        if right[vertex] != -1:
            result_right[at] = position[keys[right[vertex]]]
    return position[keys[root]] if root != -1 else -1, result_left, result_right


def valid_case(
    root: int,
    left: list[int],
    right: list[int],
    keys: list[int],
) -> Case:
    preorder, inorder, postorder = traversals(root, left, right, keys)
    return preorder, inorder, postorder, expected_tree(root, left, right, keys, inorder)


def random_tree(rng: random.Random, n: int) -> tuple[int, list[int], list[int]]:
    if n == 0:
        return -1, [], []
    left = [-1] * n
    right = [-1] * n
    available = [0]
    for vertex in range(1, n):
        slot_index = rng.randrange(len(available))
        parent = available[slot_index]
        free_sides = []
        if left[parent] == -1:
            free_sides.append(0)
        if right[parent] == -1:
            free_sides.append(1)
        side = rng.choice(free_sides)
        if side == 0:
            left[parent] = vertex
        else:
            right[parent] = vertex
        if left[parent] != -1 and right[parent] != -1:
            available[slot_index] = available[-1]
            available.pop()
        available.append(vertex)
    return 0, left, right


def append_case(lines: list[str], case: Case) -> None:
    preorder, inorder, postorder, _ = case
    lines.append(f"{len(preorder)} {len(inorder)} {len(postorder)}")
    lines.append(" ".join(map(str, preorder)))
    lines.append(" ".join(map(str, inorder)))
    lines.append(" ".join(map(str, postorder)))


def expected_line(case: Case) -> str:
    preorder, inorder, postorder, tree = case
    if tree is None:
        return "no"
    root, left, right = tree
    values = ["yes", str(len(inorder)), str(root)]
    for vertex in range(len(inorder)):
        values.extend((str(left[vertex]), str(right[vertex])))
    return " ".join(values)


def write_cases(path: Path, cases: list[Case]) -> None:
    input_lines = [str(len(cases))]
    for case in cases:
        append_case(input_lines, case)
    output_lines = [expected_line(case) for case in cases]
    (path.with_suffix(".in")).write_text("\n".join(input_lines) + "\n", encoding="utf-8")
    (path.with_suffix(".out")).write_text("\n".join(output_lines) + "\n", encoding="utf-8")


def small_cases(rng: random.Random) -> list[Case]:
    cases: list[Case] = [valid_case(-1, [], [], [])]

    sample_left = [1, 3, -1, -1, -1, -1]
    sample_right = [2, 4, 5, -1, -1, -1]
    sample_keys = [1, 2, 3, 4, 5, 6]
    sample = valid_case(0, sample_left, sample_right, sample_keys)
    cases.append(sample)

    sample_preorder, sample_inorder, _, _ = sample
    cases.append((sample_preorder, [4, 2, 5, 1, 6, 3], [4, 5, 2, 6, 1, 3], None))

    for n in range(1, 41):
        root, left, right = random_tree(rng, n)
        keys = rng.sample(range(-10**9, 10**9), n)
        valid = valid_case(root, left, right, keys)
        cases.append(valid)
        preorder, inorder, postorder, _ = valid

        if n > 1:
            rotated_postorder = postorder[1:] + postorder[:1]
            cases.append((preorder, inorder, rotated_postorder, None))

            duplicate_preorder = preorder.copy()
            duplicate_preorder[-1] = duplicate_preorder[0]
            cases.append((duplicate_preorder, inorder, postorder, None))

        changed_postorder = postorder.copy()
        changed_postorder[-1] = 3_000_000_000 + n
        cases.append((preorder, inorder, changed_postorder, None))

    cases.extend(
        [
            ([1, 2], [1, 2, 3], [2, 1], None),
            ([1, 2, 3], [1, 2, 3], [3, 2], None),
            ([1, 2, 3], [1, 1, 3], [1, 3, 2], None),
            ([1, 2, 3], [1, 2, 3], [3, 3, 1], None),
        ]
    )
    return cases


def performance_cases() -> list[Case]:
    n = 200000
    keys = [10**12 + index * 17 for index in range(n)]
    preorder = keys.copy()
    inorder = keys.copy()
    postorder = list(reversed(keys))
    left = [-1] * n
    right = [index + 1 for index in range(n)]
    right[-1] = -1
    valid: Case = (preorder, inorder, postorder, (0, left, right))

    invalid_postorder = postorder.copy()
    invalid_postorder[0], invalid_postorder[1] = invalid_postorder[1], invalid_postorder[0]
    invalid: Case = (preorder, inorder, invalid_postorder, None)
    return [valid, invalid]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    output_directory = Path(args.out_dir)
    output_directory.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260714)
    write_cases(output_directory / "case_00", small_cases(rng))
    write_cases(output_directory / "case_01", performance_cases())


if __name__ == "__main__":
    main()
