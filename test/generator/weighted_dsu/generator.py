#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


class Naive:
    def __init__(self, n: int) -> None:
        self.groups = n
        self.component = list(range(n))
        self.potential = [0] * n

    def merge(self, u: int, v: int, w: int) -> bool:
        if self.component[u] == self.component[v]:
            return self.potential[v] - self.potential[u] == w
        old = self.component[v]
        new = self.component[u]
        add = self.potential[u] + w - self.potential[v]
        for i, comp in enumerate(self.component):
            if comp == old:
                self.component[i] = new
                self.potential[i] += add
        self.groups -= 1
        return True

    def size(self, v: int) -> int:
        return sum(comp == self.component[v] for comp in self.component)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20261116)
    n = 24
    state = Naive(n)
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(350):
        typ = rng.randrange(7)
        if typ < 3:
            u = rng.randrange(n)
            v = rng.randrange(n)
            w = rng.randrange(-40, 41)
            commands.append(f"MERGE {u} {v} {w}")
            outputs.append(str(int(state.merge(u, v, w))))
        elif typ == 3:
            u = rng.randrange(n)
            v = rng.randrange(n)
            commands.append(f"SAME {u} {v}")
            outputs.append(str(int(state.component[u] == state.component[v])))
        elif typ == 4:
            u = rng.randrange(n)
            v = rng.randrange(n)
            commands.append(f"DIFF {u} {v}")
            if state.component[u] == state.component[v]:
                outputs.append(str(state.potential[v] - state.potential[u]))
            else:
                outputs.append("NA")
        elif typ == 5:
            commands.append("GROUPS")
            outputs.append(str(state.groups))
        else:
            v = rng.randrange(n)
            commands.append(f"SIZE {v}")
            outputs.append(str(state.size(v)))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
