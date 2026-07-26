

#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def optional(value: int | None) -> str:
    return "NONE" if value is None else str(value)


def select(values: list[int], value: int, occurrence: int) -> int:
    positions = [index for index, item in enumerate(values) if item == value]
    return positions[occurrence] if occurrence < len(positions) else len(values)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    n = 223
    values = [rng.randrange(-300, 301) for _ in range(n)]
    weights = [rng.randrange(-1000, 1001) for _ in range(n)]
    initial_values, initial_weights = values[:], weights[:]
    commands: list[str] = []
    outputs: list[str] = []
    kinds = [
        "SET", "SETV", "SETW", "INSERT", "ERASE", "PUSH", "POP",
        "GET", "SUM", "FREQ", "RSUM", "KTH", "KLARG", "KSMALL",
        "KLARGE", "PREV", "NEXT", "RANK", "RANKP", "SELECT", "ACCESS",
        "FREQP", "RSUMP", "SIZE", "COPY", "MOVE", "SPECIAL",
    ]

    for _ in range(1500):
        kind = rng.choice(kinds)
        current_n = len(values)
        if kind in {"ERASE", "POP"} and current_n <= 1:
            kind = "GET"
        if kind in {"INSERT", "PUSH"} and current_n >= 500:
            kind = "GET"
        if kind == "SET":
            k, value, weight = rng.randrange(current_n), rng.randrange(-350, 351), rng.randrange(-1200, 1201)
            commands.append(f"SET {k} {value} {weight}")
            values[k], weights[k] = value, weight
        elif kind == "INSERT":
            k = rng.randrange(current_n + 1)
            value, weight = rng.randrange(-350, 351), rng.randrange(-1200, 1201)
            commands.append(f"INSERT {k} {value} {weight}")
            values.insert(k, value)
            weights.insert(k, weight)
        elif kind == "ERASE":
            k = rng.randrange(current_n)
            commands.append(f"ERASE {k}")
            outputs.append(f"{values[k]} {weights[k]}")
            values.pop(k)
            weights.pop(k)
        elif kind == "PUSH":
            value, weight = rng.randrange(-350, 351), rng.randrange(-1200, 1201)
            commands.append(f"PUSH {value} {weight}")
            values.append(value)
            weights.append(weight)
        elif kind == "POP":
            commands.append("POP")
            outputs.append(f"{values[-1]} {weights[-1]}")
            values.pop()
            weights.pop()
        elif kind == "SETV":
            k, value = rng.randrange(current_n), rng.randrange(-350, 351)
            commands.append(f"SETV {k} {value}")
            values[k] = value
        elif kind == "SETW":
            k, weight = rng.randrange(current_n), rng.randrange(-1200, 1201)
            commands.append(f"SETW {k} {weight}")
            weights[k] = weight
        elif kind == "GET":
            k = rng.randrange(current_n)
            commands.append(f"GET {k}")
            outputs.append(f"{values[k]} {weights[k]}")
        elif kind == "RANK":
            value = rng.randrange(-400, 401)
            l, r = sorted((rng.randrange(current_n + 1), rng.randrange(current_n + 1)))
            commands.append(f"RANK {value} {l} {r}")
            outputs.append(str(values[l:r].count(value)))
        elif kind == "SELECT":
            value = rng.randrange(-400, 401)
            occurrence = rng.randrange(current_n + 20)
            commands.append(f"SELECT {value} {occurrence}")
            outputs.append(str(select(values, value, occurrence)))
        elif kind == "SIZE":
            commands.append("SIZE")
            outputs.append(str(current_n))
        elif kind == "COPY":
            commands.append("COPY")
            outputs.append(
                f"{current_n} {sum(weights)} {weights[0]} "
                f"{weights[0] + 1} {weights[0] - 1}"
            )
        elif kind == "MOVE":
            commands.append("MOVE")
            outputs.append(
                f"{current_n} {sum(weights)} {values[0]} {weights[0]} "
                f"{values[-1]} {weights[-1]}"
            )
        elif kind == "ACCESS":
            k = rng.randrange(current_n)
            commands.append(f"ACCESS {k}")
            outputs.append(str(values[k]))
        elif kind == "RANKP":
            value = rng.randrange(-400, 401)
            r = rng.randrange(current_n + 1)
            commands.append(f"RANKP {value} {r}")
            outputs.append(str(values[:r].count(value)))
        elif kind in {"FREQP", "RSUMP"}:
            l, r = sorted((rng.randrange(current_n + 1), rng.randrange(current_n + 1)))
            upper = rng.randrange(-400, 401)
            commands.append(f"{kind} {l} {r} {upper}")
            indices = [i for i in range(l, r) if values[i] < upper]
            outputs.append(str(len(indices) if kind == "FREQP" else sum(weights[i] for i in indices)))
        elif kind == "SPECIAL":
            commands.append("SPECIAL")
            outputs.append("3 15 -2 3 1 7")
        elif kind == "SUM":
            l, r = sorted((rng.randrange(current_n + 1), rng.randrange(current_n + 1)))
            commands.append(f"SUM {l} {r}")
            outputs.append(str(sum(weights[l:r])))
        elif kind in {"FREQ", "RSUM"}:
            l, r = sorted((rng.randrange(current_n + 1), rng.randrange(current_n + 1)))
            lower, upper = sorted((rng.randrange(-400, 401), rng.randrange(-400, 401)))
            commands.append(f"{kind} {l} {r} {lower} {upper}")
            indices = [i for i in range(l, r) if lower <= values[i] < upper]
            outputs.append(str(len(indices) if kind == "FREQ" else sum(weights[i] for i in indices)))
        elif kind in {"KTH", "KLARG", "KSMALL", "KLARGE"}:
            l = rng.randrange(current_n)
            r = rng.randrange(l + 1, current_n + 1)
            order = sorted(range(l, r), key=lambda i: values[i])
            if kind in {"KTH", "KLARG"}:
                k = rng.randrange(len(order))
                commands.append(f"{kind} {l} {r} {k}")
                outputs.append(str(values[order[k]] if kind == "KTH" else values[order[-1 - k]]))
            else:
                k = rng.randrange(len(order) + 1)
                commands.append(f"{kind} {l} {r} {k}")
                chosen = order[:k] if kind == "KSMALL" else order[len(order) - k :]
                outputs.append(str(sum(weights[i] for i in chosen)))
        else:
            l, r = sorted((rng.randrange(current_n + 1), rng.randrange(current_n + 1)))
            value = rng.randrange(-400, 401)
            commands.append(f"{kind} {l} {r} {value}")
            candidates = [item for item in values[l:r] if item < value] if kind == "PREV" else [item for item in values[l:r] if item >= value]
            answer = max(candidates) if kind == "PREV" and candidates else min(candidates) if candidates else None
            outputs.append(optional(answer))

    input_text = f"{n} {len(commands)}\n" + " ".join(map(str, initial_values)) + "\n" + " ".join(map(str, initial_weights)) + "\n" + "\n".join(commands) + "\n"
    (out_dir / "case_00.in").write_text(input_text, encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")

    boundary_values = [index % 17 - 8 for index in range(510)]
    boundary_weights = [index - 255 for index in range(510)]
    boundary_negative_sum = -1000 + sum(
        weight
        for value, weight in zip(boundary_values, boundary_weights)
        if value < 0
    )
    boundary_commands = [
        "PUSH 9 1000",
        "INSERT 0 -9 -1000",
        "SIZE",
        "ACCESS 0",
        "RANKP -9 512",
        "FREQP 0 512 0",
        "RSUMP 0 512 0",
        "SPECIAL",
        "RANK -9 0 512",
        "SELECT -9 0",
        "COPY",
        "MOVE",
        "ERASE 0",
        "POP",
        "SIZE",
    ]
    boundary_outputs = [
        "512",
        "-9",
        "1",
        "241",
        str(boundary_negative_sum),
        "3 15 -2 3 1 7",
        "1",
        "0",
        "512 -255 -1000 -999 -1001",
        "512 -255 -9 -1000 9 1000",
        "-9 -1000",
        "9 1000",
        "510",
    ]
    boundary_input = (
        f"510 {len(boundary_commands)}\n"
        + " ".join(map(str, boundary_values))
        + "\n"
        + " ".join(map(str, boundary_weights))
        + "\n"
        + "\n".join(boundary_commands)
        + "\n"
    )
    (out_dir / "case_01.in").write_text(boundary_input, encoding="utf-8")
    (out_dir / "case_01.out").write_text(
        "\n".join(boundary_outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
