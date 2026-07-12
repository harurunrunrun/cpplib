#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_UPDATE = 512


def build_case(seed: int, n: int) -> tuple[str, str]:
    rng = random.Random(seed)
    initial = [rng.randrange(-40, 41) for _ in range(n)]
    history = [initial[:]]
    commands: list[str] = []
    outputs: list[str] = []

    def add_query() -> None:
        version = rng.randrange(len(history))
        a = history[version]
        typ = rng.randrange(9)
        if typ == 0:
            k = rng.randrange(n)
            commands.append(f"ACCESS {version} {k}")
            outputs.append(str(a[k]))
        elif typ == 1:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            value = rng.randrange(-50, 51)
            commands.append(f"RANK {version} {value} {l} {r}")
            outputs.append(str(sum(value == x for x in a[l:r])))
        elif typ == 2:
            value = rng.randrange(-45, 46)
            occurrence = rng.randrange(sum(value == x for x in a) + 2)
            positions = [i for i, x in enumerate(a) if x == value]
            commands.append(f"SELECT {version} {value} {occurrence}")
            outputs.append(str(positions[occurrence] if occurrence < len(positions) else n))
        elif typ in (3, 4):
            l = rng.randrange(n)
            r = rng.randrange(l + 1, n + 1)
            k = rng.randrange(r - l)
            ordered = sorted(a[l:r])
            name = "KTH_SMALL" if typ == 3 else "KTH_LARGE"
            expected = ordered[k] if typ == 3 else ordered[-1 - k]
            commands.append(f"{name} {version} {l} {r} {k}")
            outputs.append(str(expected))
        elif typ == 5:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            upper = rng.randrange(-50, 51)
            commands.append(f"FREQ {version} {l} {r} {upper}")
            outputs.append(str(sum(x < upper for x in a[l:r])))
        elif typ == 6:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            lower, upper = sorted((rng.randrange(-50, 51), rng.randrange(-50, 51)))
            commands.append(f"FREQ_RANGE {version} {l} {r} {lower} {upper}")
            outputs.append(str(sum(lower <= x < upper for x in a[l:r])))
        else:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            border = rng.randrange(-50, 51)
            values = a[l:r]
            if typ == 7:
                candidates = [x for x in values if x < border]
                commands.append(f"PREV {version} {l} {r} {border}")
                outputs.append(str(max(candidates)) if candidates else "NONE")
            else:
                candidates = [x for x in values if border <= x]
                commands.append(f"NEXT {version} {l} {r} {border}")
                outputs.append(str(min(candidates)) if candidates else "NONE")

    for version in range(1, MAX_UPDATE + 1):
        k = rng.randrange(n)
        value = rng.randrange(-40, 41)
        next_state = history[-1][:]
        next_state[k] = value
        history.append(next_state)
        commands.append(f"SET {k} {value}")
        outputs.append(str(version))
        add_query()
        if version % 3 == 0:
            add_query()

    k = rng.randrange(n)
    commands.append(f"TRY_SET {k} {rng.randrange(-40, 41)}")
    outputs.append(f"1 {len(history)} {history[-1][k]} {history[0][k]}")
    commands.append("VERSIONS")
    outputs.append(str(len(history)))
    for _ in range(30):
        add_query()

    input_text = (
        f"{n} {len(commands)}\n"
        + " ".join(map(str, initial))
        + "\n"
        + "\n".join(commands)
        + "\n"
    )
    return input_text, "\n".join(outputs) + "\n"

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    for case_id, (seed, n) in enumerate(((2026071301, 73), (2026071302, 1))):
        input_text, output_text = build_case(seed, n)
        (out_dir / f"case_{case_id:02d}.in").write_text(input_text, encoding="utf-8")
        (out_dir / f"case_{case_id:02d}.out").write_text(output_text, encoding="utf-8")


if __name__ == "__main__":
    main()
