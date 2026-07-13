#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    out_dir = Path(parser.parse_args().out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    cases = [
        ([(0, 0, 0), (20, 0, 0), (0, 20, 0), (0, 0, 20), (20, 20, 20)], (2, -3, 1)),
        ([(-30, 10, 5), (-30, 10, 5), (30, -10, 15), (10, 40, -20)], (-1, 2, 3)),
        ([], (0, 0, 0)),
    ]
    for case_id, (source, translation) in enumerate(cases):
        target = [tuple(point[i] + translation[i] for i in range(3)) for point in source]
        lines = [f"{len(source)} {len(target)} 50 1e-18 {translation[0]} {translation[1]} {translation[2]}"]
        lines.extend(" ".join(map(str, point)) for point in source)
        lines.extend(" ".join(map(str, point)) for point in target)
        (out_dir / f"case_{case_id:02}.in").write_text("\n".join(lines) + "\n", encoding="utf-8")
        (out_dir / f"case_{case_id:02}.out").write_text(
            f"{translation[0]} {translation[1]} {translation[2]} 0\n", encoding="utf-8"
        )


if __name__ == "__main__":
    main()
