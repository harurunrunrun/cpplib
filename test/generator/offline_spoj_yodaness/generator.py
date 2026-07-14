#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, groups):
    lines = [str(len(groups))]
    output = []
    for yoda, normal in groups:
        position = {word: i for i, word in enumerate(normal)}
        sequence = [position[word] for word in yoda]
        inversions = sum(sequence[i] > sequence[j]
                         for i in range(len(sequence))
                         for j in range(i + 1, len(sequence)))
        lines += [str(len(yoda)), " ".join(yoda), " ".join(normal)]
        output.append(str(inversions))
    (out_dir / f"case_{index:02d}.in").write_text("\n".join(lines) + "\n")
    (out_dir / f"case_{index:02d}.out").write_text("\n".join(output) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [[(["force"], ["force"])],
             [(["you", "must", "learn"], ["learn", "you", "must"])]]
    rng = random.Random(730022)
    for _ in range(10):
        groups = []
        for n in range(1, 15):
            normal = [f"word_{n}_{i}" for i in range(n)]
            yoda = normal[:]
            rng.shuffle(yoda)
            groups.append((yoda, normal))
        cases.append(groups)
    for index, groups in enumerate(cases):
        write_case(out_dir, index, groups)


if __name__ == "__main__":
    main()
