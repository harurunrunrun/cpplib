#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def optional(value: int | None) -> str:
    return "NONE" if value is None else str(value)


def make_case(
    out_dir: Path,
    case_id: int,
    seed: int,
    initial_size: int,
    operation_count: int,
) -> None:
    rng = random.Random(seed)
    values = [rng.randrange(-1000, 1001) for _ in range(initial_size)]
    initial = values[:]
    commands: list[str] = []
    outputs: list[str] = []
    query_kinds = [
        "ACCESS",
        "RANK",
        "SELECT",
        "KTH",
        "KLARG",
        "FREQ",
        "RANGE",
        "PREV",
        "NEXT",
    ]

    for step in range(operation_count):
        mutation = rng.randrange(100)
        if (not values or mutation < 18) and len(values) < 3500:
            position = rng.randrange(len(values) + 1)
            value = rng.randrange(-1200, 1201)
            commands.append(f"INSERT {position} {value}")
            values.insert(position, value)
        elif mutation < 31 and values:
            position = rng.randrange(len(values))
            commands.append(f"ERASE {position}")
            outputs.append(str(values.pop(position)))
        elif mutation < 49 and values:
            position = rng.randrange(len(values))
            value = rng.randrange(-1200, 1201)
            commands.append(f"SET {position} {value}")
            values[position] = value
        else:
            kind = rng.choice(query_kinds)
            if kind == "ACCESS":
                if not values:
                    continue
                position = rng.randrange(len(values))
                commands.append(f"ACCESS {position}")
                outputs.append(str(values[position]))
            elif kind == "RANK":
                value = rng.randrange(-1400, 1401)
                left, right = sorted(
                    (rng.randrange(len(values) + 1), rng.randrange(len(values) + 1))
                )
                commands.append(f"RANK {value} {left} {right}")
                outputs.append(str(values[left:right].count(value)))
            elif kind == "SELECT":
                value = rng.randrange(-1400, 1401)
                occurrence = rng.randrange(8)
                commands.append(f"SELECT {value} {occurrence}")
                positions = [
                    index for index, item in enumerate(values) if item == value
                ]
                outputs.append(
                    str(
                        positions[occurrence]
                        if occurrence < len(positions)
                        else -1
                    )
                )
            elif kind in {"KTH", "KLARG"}:
                if not values:
                    continue
                left = rng.randrange(len(values))
                right = rng.randrange(left + 1, len(values) + 1)
                ordered = sorted(values[left:right])
                kth = rng.randrange(len(ordered))
                commands.append(f"{kind} {left} {right} {kth}")
                answer = ordered[kth] if kind == "KTH" else ordered[-1 - kth]
                outputs.append(str(answer))
            elif kind == "FREQ":
                left, right = sorted(
                    (rng.randrange(len(values) + 1), rng.randrange(len(values) + 1))
                )
                upper = rng.randrange(-1400, 1401)
                commands.append(f"FREQ {left} {right} {upper}")
                outputs.append(
                    str(sum(item < upper for item in values[left:right]))
                )
            elif kind == "RANGE":
                left, right = sorted(
                    (rng.randrange(len(values) + 1), rng.randrange(len(values) + 1))
                )
                lower, upper = sorted(
                    (rng.randrange(-1400, 1401), rng.randrange(-1400, 1401))
                )
                commands.append(
                    f"RANGE {left} {right} {lower} {upper}"
                )
                outputs.append(
                    str(
                        sum(
                            lower <= item < upper
                            for item in values[left:right]
                        )
                    )
                )
            else:
                left, right = sorted(
                    (rng.randrange(len(values) + 1), rng.randrange(len(values) + 1))
                )
                value = rng.randrange(-1400, 1401)
                commands.append(f"{kind} {left} {right} {value}")
                if kind == "PREV":
                    candidates = [
                        item for item in values[left:right] if item < value
                    ]
                    answer = max(candidates) if candidates else None
                else:
                    candidates = [
                        item for item in values[left:right] if item >= value
                    ]
                    answer = min(candidates) if candidates else None
                outputs.append(optional(answer))

        if step % 503 == 0 and values:
            commands.append("ACCESS 0")
            outputs.append(str(values[0]))
            commands.append(f"ACCESS {len(values) - 1}")
            outputs.append(str(values[-1]))

    input_text = (
        f"{len(initial)} {len(commands)}\n"
        + (" ".join(map(str, initial)) + "\n" if initial else "\n")
        + "\n".join(commands)
        + "\n"
    )
    (out_dir / f"case_{case_id:02d}.in").write_text(
        input_text, encoding="utf-8"
    )
    (out_dir / f"case_{case_id:02d}.out").write_text(
        "\n".join(outputs) + ("\n" if outputs else ""),
        encoding="utf-8",
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    make_case(out_dir, 0, 20260713, 257, 3000)
    make_case(out_dir, 1, 20260722, 0, 3500)
    make_case(out_dir, 2, 20260723, 1200, 8000)


if __name__ == "__main__":
    main()
