#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import runpy
import sys
from pathlib import Path

def require_direct_case() -> None:
    case_dir: Path | None = None
    for index, argument in enumerate(sys.argv):
        if argument == "--case-dir" and index + 1 < len(sys.argv):
            case_dir = Path(sys.argv[index + 1])
        elif argument.startswith("--case-dir="):
            case_dir = Path(argument.split("=", 1)[1])
    if case_dir is not None and not (
        case_dir / "direct_general_position.in"
    ).is_file():
        raise SystemExit(
            "missing Minkowski direct-construction regression case"
        )


require_direct_case()
runpy.run_path(
    str(Path(__file__).resolve().parents[3] / "scripts" / "standalone_checker.py"),
    run_name="__main__",
)
