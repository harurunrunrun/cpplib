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

    def copy(self) -> "Naive":
        other = Naive(0)
        other.groups = self.groups
        other.component = self.component[:]
        return other

    def merge(self, u: int, v: int) -> bool:
        if self.component[u] == self.component[v]:
            return False
        old = self.component[v]
        new = self.component[u]
        self.component = [new if x == old else x for x in self.component]
        self.groups -= 1
        return True

    def size(self, v: int) -> int:
        return sum(x == self.component[v] for x in self.component)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20261116)
    n = 24
    state = Naive(n)
    states: list[Naive] = [state.copy()]
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(350):
        typ = rng.randrange(8)
        if typ < 3:
            u = rng.randrange(n)
            v = rng.randrange(n)
            commands.append(f"MERGE {u} {v}")
            outputs.append(str(int(state.merge(u, v))))
            states.append(state.copy())
        elif typ == 3 and len(states) > 1:
            commands.append("UNDO")
            states.pop()
            state = states[-1].copy()
        elif typ == 4:
            snap = rng.randrange(len(states))
            commands.append(f"ROLLBACK {snap}")
            state = states[snap].copy()
            states = states[:snap + 1]
        elif typ == 5:
            u = rng.randrange(n)
            v = rng.randrange(n)
            commands.append(f"SAME {u} {v}")
            outputs.append(str(int(state.component[u] == state.component[v])))
        elif typ == 6:
            v = rng.randrange(n)
            commands.append(f"SIZE {v}")
            outputs.append(str(state.size(v)))
        else:
            commands.append("GROUPS")
            outputs.append(str(state.groups))

    commands.append("SNAP")
    outputs.append(str(len(states) - 1))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
