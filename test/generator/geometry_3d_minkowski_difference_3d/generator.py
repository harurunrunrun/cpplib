#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "geometry_3d_common"))
from generator_utils import generate

def append_direct_case() -> None:
    out_dir: Path | None = None
    for index, argument in enumerate(sys.argv):
        if argument == "--out-dir" and index + 1 < len(sys.argv):
            out_dir = Path(sys.argv[index + 1])
        elif argument.startswith("--out-dir="):
            out_dir = Path(argument.split("=", 1)[1])
    if out_dir is None:
        raise SystemExit("--out-dir is required")
    (out_dir / "direct_general_position.in").write_text(
        "7640891576956012809 160\n",
        encoding="utf-8",
    )
    (out_dir / "direct_general_position.out").write_text(
        "OK\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    generate()
    append_direct_case()
