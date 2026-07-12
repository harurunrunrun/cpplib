#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import random
from pathlib import Path


def random_word(rng: random.Random) -> str:
    return "".join(rng.choice("abcd") for _ in range(rng.randrange(1, 8)))


def prefix_count(values: dict[str, int], prefix: str) -> int:
    return sum(count for word, count in values.items() if word.startswith(prefix))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    rng = random.Random(2026071303)
    versions: list[dict[str, int]] = [{}]
    commands: list[str] = []
    outputs: list[str] = []

    for _ in range(600):
        typ = rng.randrange(10)
        if typ < 3:
            base = rng.randrange(len(versions))
            word = random_word(rng)
            next_values = versions[base].copy()
            next_values[word] = next_values.get(word, 0) + 1
            versions.append(next_values)
            commands.append(f"INSERT {base} {word}")
            outputs.append(str(len(versions) - 1))
        elif typ < 5:
            base = rng.randrange(len(versions))
            word = random_word(rng)
            next_values = versions[base].copy()
            if next_values.get(word, 0) > 0:
                next_values[word] -= 1
            versions.append(next_values)
            commands.append(f"ERASE {base} {word}")
            outputs.append(str(len(versions) - 1))
        elif typ == 5:
            base = rng.randrange(len(versions))
            versions.append(versions[base].copy())
            commands.append(f"FORK {base}")
            outputs.append(str(len(versions) - 1))
        elif typ == 6:
            version = rng.randrange(len(versions))
            word = random_word(rng)
            commands.append(f"COUNT {version} {word}")
            outputs.append(str(versions[version].get(word, 0)))
        elif typ == 7:
            version = rng.randrange(len(versions))
            word = random_word(rng)
            commands.append(f"CONTAINS {version} {word}")
            outputs.append(str(int(versions[version].get(word, 0) > 0)))
        else:
            version = rng.randrange(len(versions))
            word = random_word(rng)
            prefix = word[: rng.randrange(1, len(word) + 1)]
            commands.append(f"PREFIX {version} {prefix}")
            outputs.append(str(prefix_count(versions[version], prefix)))

    (out_dir / "case_00.in").write_text(
        f"{len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()