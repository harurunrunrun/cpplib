#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def palindromes(text: str) -> list[str]:
    return sorted({text[left:right] for left in range(len(text)) for right in range(left + 1, len(text) + 1) if text[left:right] == text[left:right][::-1]})


def suffix_lengths(text: str) -> list[int]:
    return [length for length in range(len(text), 0, -1) if text[-length:] == text[-length:][::-1]]


def write_case(out_dir: Path, index: int, seed: int, operation_count: int) -> None:
    rng = random.Random(seed)
    versions = [""]
    lines: list[str] = []
    output: list[str] = []
    for _ in range(operation_count):
        if len(versions) < 2 or (len(versions) < 900 and rng.randrange(100) < 52):
            base = rng.randrange(len(versions))
            c = rng.choice("abc")
            lines.append(f"A {base} {c}")
            versions.append(versions[base] + c)
            output.append(str(len(versions) - 1))
            continue
        version = rng.randrange(len(versions))
        text = versions[version]
        choice = rng.randrange(8)
        if choice == 0:
            lines.append(f"D {version}")
            output.append(str(len(palindromes(text))))
        elif choice == 1:
            lines.append(f"C {version}")
            output.append(str(sum(1 for left in range(len(text)) for right in range(left + 1, len(text) + 1) if text[left:right] == text[left:right][::-1])))
        elif choice == 2:
            lines.append(f"L {version}")
            lengths = suffix_lengths(text)
            output.append(str(lengths[0] if lengths else 0))
        elif choice == 3:
            lines.append(f"M {version}")
            output.append(str(max((len(value) for value in palindromes(text)), default=0)))
        elif choice == 4:
            lines.append(f"S {version}")
            output.append(text)
        elif choice == 5:
            if text and rng.randrange(2) == 0:
                left = rng.randrange(len(text))
                right = rng.randrange(left + 1, len(text) + 1)
                value = text[left:right]
            else:
                value = "".join(rng.choice("abc") for _ in range(rng.randint(1, 8)))
            lines.append(f"H {version} {value}")
            output.append(str(int(value == value[::-1] and value in text)))
        elif choice == 6:
            lines.append(f"T {version}")
            lengths = suffix_lengths(text)
            output.append(" ".join([str(len(lengths)), *map(str, lengths)]))
        else:
            lines.append(f"P {version}")
            lengths = sorted(map(len, palindromes(text)))
            output.append(" ".join([str(len(lengths)), *map(str, lengths)]))
    stem = f"case_{index:02d}"
    (out_dir / f"{stem}.in").write_text(str(len(lines)) + "\n" + "\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / f"{stem}.out").write_text("\n".join(output) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index in range(8):
        write_case(out_dir, index, 0xE377 + index, 320)


if __name__ == "__main__":
    main()
