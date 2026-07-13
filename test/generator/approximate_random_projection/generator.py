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
    point_count = 19
    input_dimension = 15
    output_dimension = 48
    lines = [f"{point_count} {input_dimension} {output_dimension} 2026071318"]
    for point in range(point_count):
        values = [((point + 3) * (coordinate + 5) % 37) - 18 for coordinate in range(input_dimension)]
        lines.append(" ".join(map(str, values)))
    (out_dir / "case_00.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text(
        f"{point_count} {output_dimension}\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
