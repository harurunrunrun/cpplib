#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


class Oracle:
    def __init__(self, capacity: int) -> None:
        self.capacity = capacity
        self.versions: list[list[int]] = [[]]
        self.commands: list[str] = []
        self.outputs: list[str] = []

    def push(self, version: int, value: int) -> None:
        assert len(self.versions[version]) < self.capacity
        self.versions.append(self.versions[version] + [value])
        self.commands.append(f"PUSH {version} {value}")
        self.outputs.append(str(len(self.versions) - 1))

    def pop(self, version: int) -> None:
        assert self.versions[version]
        self.versions.append(self.versions[version][1:])
        self.commands.append(f"POP {version}")
        self.outputs.append(str(len(self.versions) - 1))

    def front(self, version: int) -> None:
        assert self.versions[version]
        self.commands.append(f"FRONT {version}")
        self.outputs.append(str(self.versions[version][0]))

    def size(self, version: int) -> None:
        self.commands.append(f"SIZE {version}")
        self.outputs.append(str(len(self.versions[version])))

    def empty(self, version: int) -> None:
        self.commands.append(f"EMPTY {version}")
        self.outputs.append(str(int(not self.versions[version])))

    def versions_count(self) -> None:
        self.commands.append("VERSIONS")
        self.outputs.append(str(len(self.versions)))

    def latest(self) -> None:
        self.commands.append("LATEST")
        self.outputs.append(str(len(self.versions) - 1))

    def report_capacity(self) -> None:
        self.commands.append("CAPACITY")
        self.outputs.append(str(self.capacity))


def write_case(out_dir: Path, name: str, oracle: Oracle) -> None:
    (out_dir / f"{name}.in").write_text(
        f"{oracle.capacity} {len(oracle.commands)}\n"
        + "\n".join(oracle.commands)
        + "\n",
        encoding="utf-8",
    )
    (out_dir / f"{name}.out").write_text(
        "\n".join(oracle.outputs) + "\n",
        encoding="utf-8",
    )


def deterministic_case() -> Oracle:
    oracle = Oracle(4)
    oracle.report_capacity()
    oracle.empty(0)
    oracle.push(0, 10)
    oracle.push(1, -3)
    oracle.push(1, 7)
    oracle.front(2)
    oracle.front(3)
    oracle.pop(2)
    oracle.front(4)
    oracle.push(4, 20)
    oracle.pop(5)
    oracle.push(6, 30)
    oracle.front(7)
    oracle.size(7)
    oracle.front(2)
    oracle.versions_count()
    oracle.latest()
    return oracle


def random_case(seed: int) -> Oracle:
    rng = random.Random(seed)
    oracle = Oracle(64)
    for _ in range(700):
        kind = rng.randrange(10)
        if kind < 3:
            candidates = [
                index
                for index, queue in enumerate(oracle.versions)
                if len(queue) < oracle.capacity
            ]
            oracle.push(rng.choice(candidates), rng.randrange(-10**12, 10**12))
        elif kind < 5:
            candidates = [
                index for index, queue in enumerate(oracle.versions) if queue
            ]
            if candidates:
                oracle.pop(rng.choice(candidates))
            else:
                oracle.push(0, rng.randrange(-100, 101))
        elif kind < 7:
            candidates = [
                index for index, queue in enumerate(oracle.versions) if queue
            ]
            if candidates:
                oracle.front(rng.choice(candidates))
            else:
                oracle.empty(0)
        elif kind == 7:
            oracle.size(rng.randrange(len(oracle.versions)))
        elif kind == 8:
            oracle.empty(rng.randrange(len(oracle.versions)))
        else:
            oracle.versions_count()
    oracle.latest()
    oracle.report_capacity()
    return oracle


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    write_case(out_dir, "case_00", deterministic_case())
    write_case(out_dir, "case_01", random_case(20260714))
    write_case(out_dir, "case_02", random_case(998244353))


if __name__ == "__main__":
    main()
