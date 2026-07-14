#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


class Node:
    def __init__(self, length, suffix_link=0, parent=0):
        self.length = length
        self.suffix_link = suffix_link
        self.parent = parent
        self.next = {}


def solve(text):
    nodes = [Node(-1), Node(0)]
    last = 1
    longest = []
    for position, character in enumerate(text):
        current = last
        while True:
            length = nodes[current].length
            if position - 1 - length >= 0 and text[position - 1 - length] == character:
                break
            current = nodes[current].suffix_link
        if character in nodes[current].next:
            last = nodes[current].next[character]
            longest.append(last - 1)
            continue
        created = len(nodes)
        nodes.append(Node(nodes[current].length + 2, parent=current))
        nodes[current].next[character] = created
        if nodes[created].length == 1:
            nodes[created].suffix_link = 1
        else:
            candidate = nodes[current].suffix_link
            while True:
                length = nodes[candidate].length
                if position - 1 - length >= 0 and text[position - 1 - length] == character:
                    break
                candidate = nodes[candidate].suffix_link
            nodes[created].suffix_link = nodes[candidate].next[character]
        last = created
        longest.append(last - 1)
    relations = [
        (-1 if node.parent == 0 else node.parent - 1,
         -1 if node.suffix_link == 0 else node.suffix_link - 1)
        for node in nodes[2:]
    ]
    return relations, longest


def write_case(out_dir, index, text):
    relations, longest = solve(text)
    output = [str(len(relations))]
    output.extend(f"{parent} {link}" for parent, link in relations)
    output.append(" ".join(map(str, longest)))
    (out_dir / f"case_{index:03d}.in").write_text(
        text + "\n", encoding="utf-8"
    )
    (out_dir / f"case_{index:03d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    cases = ["a", "aa", "ab", "aba", "abba", "abacaba", "aaaaaaa"]
    rng = random.Random(0xEE77)
    for length in range(1, 40):
        cases.append("".join(chr(ord("a") + rng.randrange(4)) for _ in range(length)))
    for index, text in enumerate(cases):
        write_case(out_dir, index, text)


if __name__ == "__main__":
    main()
