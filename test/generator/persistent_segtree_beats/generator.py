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
    rng = random.Random(2026071311)
    n = 31
    initial = [rng.randrange(-500, 501) for _ in range(n)]
    initial[:4] = [2**60, -(2**60), 2**59, -(2**59)]
    versions = [initial[:]]
    commands: list[str] = []
    outputs: list[str] = []
    for _ in range(850):
        kind = rng.randrange(12)
        if kind < 5:
            base = rng.randrange(len(versions))
            current = versions[base][:]
            if kind < 3:
                left, right = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
                value = rng.randrange(-1000, 1001)
                name = ("CHMIN", "CHMAX", "ADD")[kind]
                commands.append(f"{name} {base} {left} {right} {value}")
                for index in range(left, right):
                    if kind == 0: current[index] = min(current[index], value)
                    elif kind == 1: current[index] = max(current[index], value)
                    else: current[index] += value
            elif kind == 3:
                position = rng.randrange(n)
                value = rng.randrange(-1000, 1001)
                commands.append(f"SET {base} {position} {value}")
                current[position] = value
            else:
                commands.append(f"FORK {base}")
            versions.append(current)
            outputs.append(str(len(versions) - 1))
        else:
            version = rng.randrange(len(versions))
            current = versions[version]
            if kind == 5:
                position = rng.randrange(n)
                commands.append(f"GET {version} {position}")
                outputs.append(str(current[position]))
            elif kind < 9:
                left, right = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
                if kind != 6 and left == right: right = min(n, left + 1); left = right - 1
                name = ("SUM", "MIN", "MAX")[kind - 6]
                commands.append(f"{name} {version} {left} {right}")
                part = current[left:right]
                outputs.append(str(sum(part) if kind == 6 else min(part) if kind == 7 else max(part)))
            else:
                commands.append(f"ALL {version}")
                outputs.append(f"{sum(current)} {min(current)} {max(current)}")
    (out / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + " ".join(map(str, initial)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
