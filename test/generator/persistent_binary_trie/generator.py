#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations
import argparse
import random
from pathlib import Path

LIMIT = 1 << 20

def optional(value: int | None) -> str:
    return "NONE" if value is None else str(value)

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(2026071306)
    versions: list[list[int]] = [[]]
    commands = ["SIZE 0", "KTH 0 0 0", "MIN 0 12345", "MAX 0 54321", "LESS 0 100 7"]
    outputs = ["0", "NONE", "NONE", "NONE", "0"]

    for _ in range(1050):
        kind = rng.randrange(12)
        version = rng.randrange(len(versions))
        values = versions[version]
        value = rng.randrange(LIMIT)
        if kind < 3:
            commands.append(f"INSERT {version} {value}")
            versions.append(values + [value])
            outputs.append(str(len(versions) - 1))
        elif kind == 3:
            commands.append(f"ERASE {version} {value}")
            next_values = values.copy()
            if value in next_values:
                next_values.remove(value)
            versions.append(next_values)
            outputs.append(str(len(versions) - 1))
        elif kind == 4:
            commands.append(f"FORK {version}")
            versions.append(values.copy())
            outputs.append(str(len(versions) - 1))
        elif kind == 5:
            commands.append(f"SIZE {version}")
            outputs.append(str(len(values)))
        elif kind == 6:
            commands.append(f"COUNT {version} {value}")
            outputs.append(str(values.count(value)))
        elif kind == 7:
            commands.append(f"CONTAINS {version} {value}")
            outputs.append(str(int(value in values)))
        elif kind == 8:
            xor_value = rng.randrange(LIMIT)
            k = rng.randrange(-4, len(values) + 5)
            commands.append(f"KTH {version} {k} {xor_value}")
            ordered = sorted(values, key=lambda item: item ^ xor_value)
            outputs.append(optional(ordered[k] if 0 <= k < len(ordered) else None))
        elif kind == 9:
            xor_value = rng.randrange(LIMIT)
            commands.append(f"MIN {version} {xor_value}")
            outputs.append(optional(min(values, key=lambda item: item ^ xor_value) if values else None))
        elif kind == 10:
            xor_value = rng.randrange(LIMIT)
            commands.append(f"MAX {version} {xor_value}")
            outputs.append(optional(max(values, key=lambda item: item ^ xor_value) if values else None))
        else:
            xor_value = rng.randrange(LIMIT)
            threshold = rng.randrange(LIMIT + 1)
            commands.append(f"LESS {version} {threshold} {xor_value}")
            outputs.append(str(sum((item ^ xor_value) < threshold for item in values)))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8"
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

if __name__ == "__main__":
    main()
