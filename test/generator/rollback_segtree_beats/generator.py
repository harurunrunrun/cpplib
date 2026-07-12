#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import random
from pathlib import Path

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out = Path(parser.parse_args().out_dir)
    out.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071313)
    n = 31
    initial = [rng.randrange(-500, 501) for _ in range(n)]
    initial[:4] = [2**60, -(2**60), 2**59, -(2**59)]
    states = [initial[:]]
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(900):
        kind = rng.randrange(13)
        if kind < 4:
            current = states[-1][:]
            if kind < 3:
                left, right = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
                value = rng.randrange(-1000, 1001)
                name = ("CHMIN", "CHMAX", "ADD")[kind]
                commands.append(f"{name} {left} {right} {value}")
                for index in range(left, right):
                    if kind == 0: current[index] = min(current[index], value)
                    elif kind == 1: current[index] = max(current[index], value)
                    else: current[index] += value
            else:
                position = rng.randrange(n)
                value = rng.randrange(-1000, 1001)
                commands.append(f"SET {position} {value}")
                current[position] = value
            states.append(current)
        elif kind == 4:
            if len(states) > 1:
                snapshot = rng.randrange(len(states))
                commands.append(f"ROLLBACK {snapshot}")
                states = states[:snapshot + 1]
            else:
                commands.append("SNAPSHOT")
                outputs.append("0")
        elif kind == 5:
            commands.append("SNAPSHOT")
            outputs.append(str(len(states) - 1))
        elif kind == 6:
            position = rng.randrange(n)
            commands.append(f"GET {position}")
            outputs.append(str(states[-1][position]))
        elif kind < 10:
            left, right = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            if kind != 7 and left == right: right = min(n, left + 1); left = right - 1
            name = ("SUM", "MIN", "MAX")[kind - 7]
            commands.append(f"{name} {left} {right}")
            part = states[-1][left:right]
            outputs.append(str(sum(part) if kind == 7 else min(part) if kind == 8 else max(part)))
        else:
            commands.append("ALL")
            current = states[-1]
            outputs.append(f"{sum(current)} {min(current)} {max(current)}")
    (out / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + " ".join(map(str, initial)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
