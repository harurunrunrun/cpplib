#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def semantic(mode: str) -> str:
    if mode.endswith("ASSIGN"):
        return "ASSIGN"
    if mode.endswith("FLIP"):
        return "FLIP"
    return "OVERWRITE"


def write_partial(out_dir: Path, index: int, mode: str, n: int, seed: int) -> None:
    rng = random.Random(seed)
    versions = [[0] * n]
    operations: list[tuple] = []
    output: list[str] = []
    kind = semantic(mode)
    for _ in range(300):
        choice = rng.randrange(6)
        left = rng.randrange(n)
        right = rng.randrange(left + 1, n + 1)
        current = versions[-1][:]
        if choice == 0 and kind == "ASSIGN":
            value = rng.randrange(2)
            operations.append(("A", left, right, value))
            current[left:right] = [value] * (right - left)
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice == 0 and kind == "FLIP":
            operations.append(("F", left, right))
            current[left:right] = [value ^ 1 for value in current[left:right]]
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice == 1 and kind == "FLIP":
            value = rng.randrange(2)
            operations.append(("A", left, right, value))
            current[left:right] = [value] * (right - left)
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice < 2 and kind == "OVERWRITE":
            operations.append(("S", left, right))
            current[left:right] = [1] * (right - left)
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice == 2:
            operations.append(("K",))
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice < 5:
            version = rng.randrange(len(versions))
            operations.append(("Q", version, left, right))
            output.append(str(sum(versions[version][left:right])))
        else:
            version = rng.randrange(len(versions))
            position = rng.randrange(n)
            operations.append(("G", version, position))
            output.append(str(versions[version][position]))
    write_files(out_dir, index, mode, n, operations, output)


def write_rollback(out_dir: Path, index: int, mode: str, n: int, seed: int) -> None:
    rng = random.Random(seed)
    versions = [[0] * n]
    operations: list[tuple] = []
    output: list[str] = []
    kind = semantic(mode)
    for _ in range(300):
        choice = rng.randrange(8)
        left = rng.randrange(n)
        right = rng.randrange(left + 1, n + 1)
        current = versions[-1][:]
        if choice == 0 and kind == "ASSIGN":
            value = rng.randrange(2)
            operations.append(("A", left, right, value))
            current[left:right] = [value] * (right - left)
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice == 0 and kind == "FLIP":
            operations.append(("F", left, right))
            current[left:right] = [value ^ 1 for value in current[left:right]]
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice == 1 and kind == "FLIP":
            value = rng.randrange(2)
            operations.append(("A", left, right, value))
            current[left:right] = [value] * (right - left)
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice < 2 and kind == "OVERWRITE":
            operations.append(("S", left, right))
            current[left:right] = [1] * (right - left)
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice == 2:
            operations.append(("K",))
            versions.append(current)
            output.append(str(len(versions) - 1))
        elif choice == 3 and len(versions) > 1:
            version = rng.randrange(len(versions))
            operations.append(("B", version))
            versions = versions[: version + 1]
            output.append(str(version))
        elif choice < 7:
            operations.append(("Q", left, right))
            output.append(str(sum(versions[-1][left:right])))
        else:
            position = rng.randrange(n)
            operations.append(("G", position))
            output.append(str(versions[-1][position]))
    write_files(out_dir, index, mode, n, operations, output)


def write_files(out_dir: Path, index: int, mode: str, n: int, operations: list[tuple], output: list[str]) -> None:
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
    partial = ("PASSIGN", "PDASSIGN", "PFLIP", "PDFLIP", "POVERWRITE", "PDOVERWRITE")
    rollback = ("RASSIGN", "RDASSIGN", "RFLIP", "RDFLIP", "ROVERWRITE", "RDOVERWRITE")
    index = 0
    for mode in partial:
        for n in (1, 65, 511):
            write_partial(out_dir, index, mode, n, 0xA771 + index)
            index += 1
    for mode in rollback:
        for n in (1, 65, 511):
            write_rollback(out_dir, index, mode, n, 0xB771 + index)
            index += 1


if __name__ == "__main__":
    main()
