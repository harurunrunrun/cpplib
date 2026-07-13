#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import math
import random
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(20260713)
    identities = [
        ("EADD", 0),
        ("EMUL", 1),
        ("EMIN", (1 << 31) - 1),
        ("EMAX", -(1 << 31)),
        ("EGCD", 0),
        ("ELCM", 1),
        ("EXOR", 0),
        ("EAND", -1),
        ("EOR", 0),
    ]
    commands = [name for name, _ in identities]
    outputs = [str(value) for _, value in identities]
    kinds = ["ADD", "MUL", "MIN", "MAX", "GCD", "LCM", "XOR", "AND", "OR"]

    for _ in range(650):
        kind = rng.choice(kinds)
        if kind == "ADD":
            a, b = rng.randrange(-10**12, 10**12), rng.randrange(-10**12, 10**12)
            result = a + b
        elif kind == "MUL":
            a, b = rng.randrange(-10**6, 10**6), rng.randrange(-10**6, 10**6)
            result = a * b
        elif kind == "MIN":
            a, b = rng.randrange(-(1 << 30), 1 << 30), rng.randrange(-(1 << 30), 1 << 30)
            result = min(a, b)
        elif kind == "MAX":
            a, b = rng.randrange(-(1 << 30), 1 << 30), rng.randrange(-(1 << 30), 1 << 30)
            result = max(a, b)
        elif kind == "GCD":
            a, b = rng.randrange(-(1 << 30), 1 << 30), rng.randrange(-(1 << 30), 1 << 30)
            result = math.gcd(a, b)
        elif kind == "LCM":
            a, b = rng.randrange(1, 30000), rng.randrange(1, 30000)
            result = math.lcm(a, b)
        else:
            a, b = rng.randrange(1 << 30), rng.randrange(1 << 30)
            result = {"XOR": a ^ b, "AND": a & b, "OR": a | b}[kind]
        commands.append(f"{kind} {a} {b}")
        outputs.append(str(result))

    action_identities = [
        ("IDADD", "0"),
        ("IDASSIGN", "0 0"),
        ("IDAFFINE", "1 0"),
        ("IDFLIP", "0"),
    ]
    for command, result in action_identities:
        commands.append(command)
        outputs.append(result)

    int64_max = (1 << 63) - 1
    int64_min = -(1 << 63)
    edge_actions = [
        (f"ADDMIN {int64_max} 7", int64_max),
        (f"ADDMAX {int64_min} -7", int64_min),
        (f"ASSIGNMIN {int64_max} 1 3", int64_max),
        (f"ASSIGNMAX {int64_min} 1 3", int64_min),
        (f"CHMINMIN {int64_max} 3", int64_max),
        (f"CHMAXMAX {int64_min} 3", int64_min),
        ("COMPOSE_ASSIGN 1 7 1 3", "1 7"),
        ("COMPOSE_AFFINE 2 5 3 7", "6 19"),
        ("COMPOSE_FLIP 1 1", 0),
    ]
    for command, result in edge_actions:
        commands.append(command)
        outputs.append(str(result))

    action_kinds = [
        "ADDMIN",
        "ADDMAX",
        "ASSIGNMIN",
        "ASSIGNMAX",
        "CHMINMIN",
        "CHMAXMAX",
        "ADDSUM",
        "MULSUM",
        "FLIPCOUNT",
        "ASSIGNSUM",
        "AFFINESUM",
        "XORXOR",
        "COMPOSE_ASSIGN",
        "COMPOSE_AFFINE",
        "COMPOSE_FLIP",
    ]
    for _ in range(900):
        kind = rng.choice(action_kinds)
        if kind in ("ADDMIN", "ADDMAX"):
            x = rng.randrange(-10**12, 10**12)
            f = rng.randrange(-10**9, 10**9)
            command = f"{kind} {x} {f}"
            result = x + f
        elif kind in ("ASSIGNMIN", "ASSIGNMAX"):
            x = rng.randrange(-10**12, 10**12)
            assigned = rng.randrange(2)
            value = rng.randrange(-10**12, 10**12)
            command = f"{kind} {x} {assigned} {value}"
            result = value if assigned else x
        elif kind == "CHMINMIN":
            x = rng.randrange(-10**12, 10**12)
            bound = rng.randrange(-10**12, 10**12)
            command = f"{kind} {x} {bound}"
            result = min(x, bound)
        elif kind == "CHMAXMAX":
            x = rng.randrange(-10**12, 10**12)
            bound = rng.randrange(-10**12, 10**12)
            command = f"{kind} {x} {bound}"
            result = max(x, bound)
        elif kind == "ADDSUM":
            length = rng.randrange(0, 1000)
            x = rng.randrange(-10**12, 10**12)
            f = rng.randrange(-10**6, 10**6)
            command = f"{kind} {x} {length} {f}"
            result = x + f * length
        elif kind == "MULSUM":
            length = rng.randrange(0, 1000)
            x = 0 if length == 0 else rng.randrange(-10**9, 10**9)
            f = rng.randrange(-1000, 1000)
            command = f"{kind} {x} {length} {f}"
            result = x * f
        elif kind == "FLIPCOUNT":
            length = rng.randrange(0, 1000)
            count = rng.randrange(length + 1)
            flip = rng.randrange(2)
            command = f"{kind} {count} {length} {flip}"
            result = length - count if flip else count
        elif kind == "ASSIGNSUM":
            length = rng.randrange(0, 1000)
            x = 0 if length == 0 else rng.randrange(-10**12, 10**12)
            assigned = rng.randrange(2)
            value = rng.randrange(-10**6, 10**6)
            command = f"{kind} {x} {length} {assigned} {value}"
            result = value * length if assigned else x
        elif kind == "AFFINESUM":
            length = rng.randrange(0, 1000)
            x = 0 if length == 0 else rng.randrange(-10**9, 10**9)
            multiplier = rng.randrange(-100, 100)
            addend = rng.randrange(-10**6, 10**6)
            command = (
                f"{kind} {x} {length} {multiplier} {addend}"
            )
            result = multiplier * x + addend * length
        elif kind == "XORXOR":
            length = rng.randrange(0, 1000)
            x = rng.randrange(1 << 30)
            mask = rng.randrange(1 << 30)
            command = f"{kind} {x} {length} {mask}"
            result = x ^ mask if length & 1 else x
        elif kind == "COMPOSE_ASSIGN":
            f_assigned = rng.randrange(2)
            f_value = rng.randrange(-10**9, 10**9)
            g_assigned = rng.randrange(2)
            g_value = rng.randrange(-10**9, 10**9)
            command = (
                f"{kind} {f_assigned} {f_value} "
                f"{g_assigned} {g_value}"
            )
            if f_assigned:
                result = f"1 {f_value}"
            elif g_assigned:
                result = f"1 {g_value}"
            else:
                result = "0 0"
        elif kind == "COMPOSE_AFFINE":
            fm = rng.randrange(-20, 20)
            fa = rng.randrange(-10**6, 10**6)
            gm = rng.randrange(-20, 20)
            ga = rng.randrange(-10**6, 10**6)
            command = f"{kind} {fm} {fa} {gm} {ga}"
            result = f"{fm * gm} {fm * ga + fa}"
        else:
            f = rng.randrange(2)
            g = rng.randrange(2)
            command = f"{kind} {f} {g}"
            result = f ^ g
        commands.append(command)
        outputs.append(str(result))

    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
