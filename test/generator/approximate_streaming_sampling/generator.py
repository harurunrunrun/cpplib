#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    count = 1200
    lines = [str(count)]
    lines.extend(f"{index * 17 + 5} {1 + index % 31}" for index in range(count))
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("OK\n", encoding="utf-8")


if __name__ == "__main__":
    main()
