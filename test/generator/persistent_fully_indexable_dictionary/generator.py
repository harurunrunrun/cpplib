#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations
import argparse
import random
from pathlib import Path

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071310)
    n = 257
    initial = [bool(rng.randrange(2)) for _ in range(n)]
    versions = [initial.copy()]
    commands: list[str] = []
    outputs: list[str] = []

    for k in [0, 63, 64, 127, 128, 256]:
        commands.append(f"GET 0 {k}")
        outputs.append(str(int(initial[k])))
    commands.extend(["RANK 0 1 0", f"RANK 0 0 {n}", f"RANGE 0 1 63 129"])
    outputs.extend([
        "0",
        str(initial.count(False)),
        str(sum(initial[63:129])),
    ])

    for _ in range(1100):
        kind = rng.randrange(12)
        version = rng.randrange(len(versions))
        bits = versions[version]
        if kind < 3:
            k, value = rng.randrange(n), bool(rng.randrange(2))
            commands.append(f"SET {version} {k} {int(value)}")
            next_bits = bits.copy()
            next_bits[k] = value
            versions.append(next_bits)
            outputs.append(str(len(versions) - 1))
        elif kind < 5:
            k = rng.randrange(n)
            commands.append(f"FLIP {version} {k}")
            next_bits = bits.copy()
            next_bits[k] = not next_bits[k]
            versions.append(next_bits)
            outputs.append(str(len(versions) - 1))
        elif kind == 5:
            commands.append(f"FORK {version}")
            versions.append(bits.copy())
            outputs.append(str(len(versions) - 1))
        elif kind == 6:
            k = rng.randrange(n)
            commands.append(f"GET {version} {k}")
            outputs.append(str(int(bits[k])))
        elif kind < 9:
            value, r = bool(rng.randrange(2)), rng.randrange(n + 1)
            commands.append(f"RANK {version} {int(value)} {r}")
            outputs.append(str(bits[:r].count(value)))
        elif kind == 9:
            value = bool(rng.randrange(2))
            l, r = rng.randrange(n + 1), rng.randrange(n + 1)
            if l > r:
                l, r = r, l
            commands.append(f"RANGE {version} {int(value)} {l} {r}")
            outputs.append(str(bits[l:r].count(value)))
        else:
            value = bool(rng.randrange(2))
            occurrence = rng.randrange(bits.count(value) + 4)
            commands.append(f"SELECT {version} {int(value)} {occurrence}")
            positions = [k for k, bit in enumerate(bits) if bit == value]
            outputs.append(str(positions[occurrence] if occurrence < len(positions) else n))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n" + "".join("1" if bit else "0" for bit in initial) + "\n"
        + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
