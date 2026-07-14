#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def compatible(preorder, inorder, postorder):
    if sorted(preorder) != sorted(inorder) or sorted(preorder) != sorted(postorder):
        return False
    position = {value: index for index, value in enumerate(inorder)}
    if len(position) != len(inorder):
        return False
    cursor = 0

    def build(left, right):
        nonlocal cursor
        if left == right:
            return []
        if cursor == len(preorder):
            raise ValueError
        root = preorder[cursor]
        cursor += 1
        middle = position.get(root, -1)
        if middle < left or middle >= right:
            raise ValueError
        return build(left, middle) + build(middle + 1, right) + [root]

    try:
        rebuilt = build(0, len(inorder))
    except ValueError:
        return False
    return cursor == len(preorder) and rebuilt == postorder


def traversals(left_child, right_child):
    preorder = []
    inorder = []
    postorder = []

    def dfs(v):
        if v == -1:
            return
        preorder.append(v + 1)
        dfs(left_child[v])
        inorder.append(v + 1)
        dfs(right_child[v])
        postorder.append(v + 1)

    dfs(0)
    return preorder, postorder, inorder


def random_tree(rng, n):
    left = [-1] * n
    right = [-1] * n
    available = [(0, 0), (0, 1)]
    for vertex in range(1, n):
        slot = rng.randrange(len(available))
        parent, side = available.pop(slot)
        if side == 0:
            left[parent] = vertex
        else:
            right[parent] = vertex
        available += [(vertex, 0), (vertex, 1)]
    return traversals(left, right)


def write_case(out_dir, index, preorder, postorder, inorder):
    lines = [str(len(preorder)), " ".join(map(str, preorder)),
             " ".join(map(str, postorder)), " ".join(map(str, inorder))]
    answer = "yes" if compatible(preorder, inorder, postorder) else "no"
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text(answer + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [([1], [1], [1]),
             ([1, 2, 3], [2, 3, 1], [2, 1, 3]),
             ([1, 2, 3], [1, 2, 3], [2, 1, 3])]
    rng = random.Random(730018)
    for n in range(2, 30):
        preorder, postorder, inorder = random_tree(rng, n)
        if n % 2 == 0:
            postorder = postorder[:]
            i, j = rng.sample(range(n), 2)
            postorder[i], postorder[j] = postorder[j], postorder[i]
        cases.append((preorder, postorder, inorder))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
