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

    rng = random.Random(2026071305)
    n = 71
    initial_values = [rng.randrange(-180, 181) for _ in range(n)]
    initial_weights = [rng.randrange(-1000, 1001) for _ in range(n)]
    versions: list[tuple[list[int], list[int]]] = [
        (initial_values[:], initial_weights[:])
    ]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(600):
        typ = rng.randrange(12)
        if typ == 0:
            base = rng.randrange(len(versions))
            k = rng.randrange(n)
            value = rng.randrange(-220, 221)
            weight = rng.randrange(-1200, 1201)
            values = versions[base][0][:]
            weights = versions[base][1][:]
            values[k] = value
            weights[k] = weight
            versions.append((values, weights))
            commands.append(f"SET {base} {k} {value} {weight}")
            outputs.append(str(len(versions) - 1))
        elif typ == 1:
            base = rng.randrange(len(versions))
            k = rng.randrange(n)
            value = rng.randrange(-220, 221)
            values = versions[base][0][:]
            weights = versions[base][1][:]
            values[k] = value
            versions.append((values, weights))
            commands.append(f"SETV {base} {k} {value}")
            outputs.append(str(len(versions) - 1))
        elif typ == 2:
            base = rng.randrange(len(versions))
            k = rng.randrange(n)
            weight = rng.randrange(-1200, 1201)
            values = versions[base][0][:]
            weights = versions[base][1][:]
            weights[k] = weight
            versions.append((values, weights))
            commands.append(f"SETW {base} {k} {weight}")
            outputs.append(str(len(versions) - 1))
        elif typ == 3:
            base = rng.randrange(len(versions))
            versions.append((versions[base][0][:], versions[base][1][:]))
            commands.append(f"FORK {base}")
            outputs.append(str(len(versions) - 1))
        else:
            version = rng.randrange(len(versions))
            values, weights = versions[version]
            left = rng.randrange(n + 1)
            right = rng.randrange(n + 1)
            if left > right:
                left, right = right, left
            if typ == 4:
                commands.append(f"SUM {version} {left} {right}")
                outputs.append(str(sum(weights[left:right])))
            elif typ < 7:
                lower = rng.randrange(-240, 241)
                upper = rng.randrange(-240, 241)
                if lower > upper:
                    lower, upper = upper, lower
                commands.append(
                    f"FREQ {version} {left} {right} {lower} {upper}"
                )
                outputs.append(
                    str(sum(lower <= values[k] < upper for k in range(left, right)))
                )
            elif typ < 9:
                lower = rng.randrange(-240, 241)
                upper = rng.randrange(-240, 241)
                if lower > upper:
                    lower, upper = upper, lower
                commands.append(
                    f"RSUM {version} {left} {right} {lower} {upper}"
                )
                outputs.append(
                    str(
                        sum(
                            weights[k]
                            for k in range(left, right)
                            if lower <= values[k] < upper
                        )
                    )
                )
            elif typ == 9:
                if left == right:
                    right = min(n, left + 1)
                    left = right - 1
                k = rng.randrange(right - left)
                ordered = sorted(values[left:right])
                commands.append(f"KTH {version} {left} {right} {k}")
                outputs.append(str(ordered[k]))
            else:
                k = rng.randrange(right - left + 1)
                order = list(range(left, right))
                order.sort(key=lambda index: values[index])
                commands.append(f"KSMALL {version} {left} {right} {k}")
                outputs.append(str(sum(weights[index] for index in order[:k])))

    (out_dir / "case_00.in").write_text(
        f"{n} {len(commands)}\n"
        + " ".join(map(str, initial_values))
        + "\n"
        + " ".join(map(str, initial_weights))
        + "\n"
        + "\n".join(commands)
        + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()