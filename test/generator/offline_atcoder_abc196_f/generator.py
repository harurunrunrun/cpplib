#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path


def write_case(out_dir, index, s, t):
    answer = min(sum(a != b for a, b in zip(s[left:left + len(t)], t))
                 for left in range(len(s) - len(t) + 1))
    (out_dir / f"case_{index:02d}.in").write_text(f"{s}\n{t}\n")
    (out_dir / f"case_{index:02d}.out").write_text(f"{answer}\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [("0", "0"), ("1", "0"), ("01010101", "101"), ("11110000", "0011")]
    rng = random.Random(19606)
    for n in range(2, 35, 4):
        m = rng.randint(1, n)
        s = "".join(rng.choice("01") for _ in range(n))
        t = "".join(rng.choice("01") for _ in range(m))
        cases.append((s, t))
    for index, case in enumerate(cases):
        write_case(out_dir, index, *case)


if __name__ == "__main__":
    main()
