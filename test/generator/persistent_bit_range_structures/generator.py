#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def write_case(out_dir: Path, index: int, mode: str, n: int, seed: int) -> None:
    rng = random.Random(seed)
    versions = [[0] * n]
    operations: list[tuple] = []
    output: list[str] = []
    for _ in range(420):
        version = rng.randrange(len(versions))
        choice = rng.randrange(6)
        left = rng.randrange(n)
        right = rng.randrange(left + 1, n + 1)
        if len(versions) >= 900:
            choice = 4
        if mode.endswith("ASSIGN") and choice < 2:
            value = rng.randrange(2)
            operations.append(("A", version, left, right, value))
            current = versions[version][:]
            current[left:right] = [value] * (right - left)
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif mode.endswith("FLIP") and choice == 0:
            operations.append(("F", version, left, right))
            current = versions[version][:]
            current[left:right] = [value ^ 1 for value in current[left:right]]
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif mode.endswith("FLIP") and choice == 1:
            value = rng.randrange(2)
            operations.append(("A", version, left, right, value))
            current = versions[version][:]
            current[left:right] = [value] * (right - left)
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif mode.endswith("OVERWRITE") and choice < 2:
            operations.append(("S", version, left, right))
            current = versions[version][:]
            current[left:right] = [1] * (right - left)
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice == 2:
            operations.append(("K", version))
            versions.append(versions[version][:])
            output.append(str(len(versions) - 1))
        elif choice < 5:
            operations.append(("Q", version, left, right))
            output.append(str(sum(versions[version][left:right])))
        else:
            position = rng.randrange(n)
            operations.append(("G", version, position))
            output.append(str(versions[version][position]))
    stem = f"case_{index:02d}"
    lines = [f"{mode} {n} {len(operations)}", *(" ".join(map(str, operation)) for operation in operations)]
    (out_dir / f"{stem}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text("\n".join(output) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    index = 0
    for mode in ("ASSIGN", "DASSIGN", "FLIP", "DFLIP", "OVERWRITE", "DOVERWRITE"):
        for n in (1, 65, 511):
            write_case(out_dir, index, mode, n, 0xB175 + index)
            index += 1


if __name__ == "__main__":
    main()
