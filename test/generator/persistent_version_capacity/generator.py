#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    commands = [
        "PFID_SET",
        "PFID_FORK",
        "PPFID_SET",
        "PWM_SET",
        "PWM_FORK",
        "PPWM_SET",
        "PWWM_SET",
        "PWWM_FORK",
        "PPWWM_SET",
    ]
    outputs = [
        "1 2 0 1 1",
        "1 2 1",
        "1 2 0 1 1",
        "1 2 1 3 2",
        "1 2 2",
        "1 2 1 3 2",
        "1 2 1 10 3 30",
        "1 2 2 20",
        "1 2 1 10 3 30",
    ]
    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text("\n".join(outputs) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
