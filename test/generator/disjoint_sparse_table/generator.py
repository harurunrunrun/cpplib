#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    n = 97
    values = [rng.randrange(1024) for _ in range(n)]
    words = [chr(ord("a") + i % 26) * (i % 3 + 1) for i in range(n)]
    commands = ["SUM 0 0", f"XOR 0 {n}", "CONCAT 0 0", f"CONCAT 0 {n}"]
    xor_all = 0
    for value in values:
        xor_all ^= value
    outputs = ["0", str(xor_all), "", "".join(words)]

    for _ in range(500):
        kind = rng.randrange(3)
        l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
        if kind == 0:
            commands.append(f"SUM {l} {r}")
            outputs.append(str(sum(values[l:r])))
        elif kind == 1:
            commands.append(f"XOR {l} {r}")
            result = 0
            for value in values[l:r]:
                result ^= value
            outputs.append(str(result))
        else:
            commands.append(f"CONCAT {l} {r}")
            outputs.append("".join(words[l:r]))

    lines = [f"{n} {len(commands)}", " ".join(map(str, values)), " ".join(words)]
    lines.extend(commands)
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
