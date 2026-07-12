#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


MAX_UPDATE = 384


def build_case(seed: int, n: int) -> tuple[str, str]:
    rng = random.Random(seed)
    initial_values = [rng.randrange(-30, 31) for _ in range(n)]
    initial_weights = [rng.randrange(-100, 101) for _ in range(n)]
    value_history = [initial_values[:]]
    weight_history = [initial_weights[:]]
    commands: list[str] = []
    outputs: list[str] = []

    def add_query() -> None:
        version = rng.randrange(len(value_history))
        values = value_history[version]
        weights = weight_history[version]
        typ = rng.randrange(14)
        if typ in (0, 1):
            k = rng.randrange(n)
            name = "ACCESS" if typ == 0 else "WEIGHT"
            commands.append(f"{name} {version} {k}")
            outputs.append(str(values[k] if typ == 0 else weights[k]))
        elif typ == 2:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            value = rng.randrange(-40, 41)
            commands.append(f"RANK {version} {value} {l} {r}")
            outputs.append(str(sum(x == value for x in values[l:r])))
        elif typ in (3, 4):
            l = rng.randrange(n)
            r = rng.randrange(l + 1, n + 1)
            k = rng.randrange(r - l)
            ordered = sorted(values[l:r])
            name = "KTH_SMALL" if typ == 3 else "KTH_LARGE"
            expected = ordered[k] if typ == 3 else ordered[-1 - k]
            commands.append(f"{name} {version} {l} {r} {k}")
            outputs.append(str(expected))
        elif typ == 5:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            upper = rng.randrange(-40, 41)
            commands.append(f"FREQ {version} {l} {r} {upper}")
            outputs.append(str(sum(x < upper for x in values[l:r])))
        elif typ == 6:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            lower, upper = sorted((rng.randrange(-40, 41), rng.randrange(-40, 41)))
            commands.append(f"FREQ_RANGE {version} {l} {r} {lower} {upper}")
            outputs.append(str(sum(lower <= x < upper for x in values[l:r])))
        elif typ == 7:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            commands.append(f"SUM {version} {l} {r}")
            outputs.append(str(sum(weights[l:r])))
        elif typ == 8:
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            lower, upper = sorted((rng.randrange(-40, 41), rng.randrange(-40, 41)))
            commands.append(f"SUM_RANGE {version} {l} {r} {lower} {upper}")
            outputs.append(
                str(sum(weights[i] for i in range(l, r) if lower <= values[i] < upper))
            )
        elif typ in (9, 10):
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            k = rng.randrange(r - l + 1)
            order = sorted(range(l, r), key=lambda i: values[i])
            if typ == 9:
                expected = sum(weights[i] for i in order[:k])
                name = "SUM_K_SMALL"
            else:
                expected = sum(weights[i] for i in order[len(order) - k :])
                name = "SUM_K_LARGE"
            commands.append(f"{name} {version} {l} {r} {k}")
            outputs.append(str(expected))
        elif typ in (11, 12):
            l, r = sorted((rng.randrange(n + 1), rng.randrange(n + 1)))
            border = rng.randrange(-40, 41)
            selected = values[l:r]
            if typ == 11:
                candidates = [x for x in selected if x < border]
                commands.append(f"PREV {version} {l} {r} {border}")
                outputs.append(str(max(candidates)) if candidates else "NONE")
            else:
                candidates = [x for x in selected if border <= x]
                commands.append(f"NEXT {version} {l} {r} {border}")
                outputs.append(str(min(candidates)) if candidates else "NONE")
        else:
            commands.append("VERSIONS")
            outputs.append(str(len(value_history)))

    for version in range(1, MAX_UPDATE + 1):
        k = rng.randrange(n)
        values = value_history[-1][:]
        weights = weight_history[-1][:]
        update_type = rng.randrange(3)
        if update_type == 0:
            value = rng.randrange(-30, 31)
            weight = rng.randrange(-100, 101)
            values[k] = value
            weights[k] = weight
            commands.append(f"SET {k} {value} {weight}")
        elif update_type == 1:
            value = rng.randrange(-30, 31)
            values[k] = value
            commands.append(f"SET_VALUE {k} {value}")
        else:
            weight = rng.randrange(-100, 101)
            weights[k] = weight
            commands.append(f"SET_WEIGHT {k} {weight}")
        value_history.append(values)
        weight_history.append(weights)
        outputs.append(str(version))
        add_query()
        if version % 2 == 0:
            add_query()

    k = rng.randrange(n)
    commands.append(f"TRY_SET {k} {rng.randrange(-30, 31)} {rng.randrange(-100, 101)}")
    outputs.append(
        f"1 {len(value_history)} {value_history[-1][k]} {weight_history[-1][k]} "
        f"{value_history[0][k]} {weight_history[0][k]}"
    )
    commands.append("VERSIONS")
    outputs.append(str(len(value_history)))
    for _ in range(40):
        add_query()

    input_text = (
        f"{n} {len(commands)}\n"
        + " ".join(map(str, initial_values))
        + "\n"
        + " ".join(map(str, initial_weights))
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

    for case_id, (seed, n) in enumerate(((2026071311, 67), (2026071312, 1))):
        input_text, output_text = build_case(seed, n)
        (out_dir / f"case_{case_id:02d}.in").write_text(input_text, encoding="utf-8")
        (out_dir / f"case_{case_id:02d}.out").write_text(output_text, encoding="utf-8")


if __name__ == "__main__":
    main()
