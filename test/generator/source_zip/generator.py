#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import base64
import random
from pathlib import Path


def varuint(value: int) -> bytes:
    result = bytearray()
    while value >= 128:
        result.append((value & 127) | 128)
        value >>= 7
    result.append(value)
    return bytes(result)


def zigzag(value: int, bits: int) -> int:
    if value >= 0:
        return value << 1
    magnitude = -value
    return ((magnitude << 1) - 1) & ((1 << bits) - 1)


def encode(values: list[int], signed: bool, bits: int) -> str:
    data = bytearray(varuint(len(values)))
    left = 0
    while left < len(values):
        right = left + 1
        while right < len(values) and values[right] == values[left]:
            right += 1
        data.extend(varuint(right - left))
        data.extend(varuint(zigzag(values[left], bits) if signed else values[left]))
        left = right
    return base64.urlsafe_b64encode(bytes(data)).decode().rstrip("=")


def write_encode(out_dir: Path, index: int, kind: str, values: list[int], signed: bool, bits: int) -> None:
    stem = f"case_{index:02d}"
    text = f"{kind}\n{len(values)}\n" + " ".join(map(str, values)) + "\n"
    (out_dir / f"{stem}.in").write_text(text, encoding="utf-8")
    (out_dir / f"{stem}.out").write_text(f"{encode(values, signed, bits)}\n1\n", encoding="utf-8")


def write_decode(out_dir: Path, index: int, kind: str, values: list[int], signed: bool, bits: int) -> None:
    stem = f"case_{index:02d}"
    encoded = encode(values, signed, bits)
    (out_dir / f"{stem}.in").write_text(f"{kind}\n{encoded}\n", encoding="utf-8")
    output = " ".join([str(len(values)), *map(str, values)]) + "\n"
    (out_dir / f"{stem}.out").write_text(output, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    rng = random.Random(0x205A1)
    signed_cases = [
        [],
        [0],
        [-1, -1, -1, 0, 1, 1],
        [-(1 << 63), (1 << 63) - 1, 0],
        [rng.randint(-10**12, 10**12) for _ in range(200)],
    ]
    unsigned_cases = [
        [0],
        [0, 0, 2**32 - 1, 2**32 - 1, 17],
        [rng.randrange(2**32) for _ in range(200)],
    ]
    index = 0
    for values in signed_cases:
        write_encode(out_dir, index, "ENCODE_I64", values, True, 64)
        index += 1
        write_decode(out_dir, index, "DECODE_I64", values, True, 64)
        index += 1
    for values in unsigned_cases:
        write_encode(out_dir, index, "ENCODE_U32", values, False, 32)
        index += 1
        write_decode(out_dir, index, "DECODE_U32", values, False, 32)
        index += 1


if __name__ == "__main__":
    main()
