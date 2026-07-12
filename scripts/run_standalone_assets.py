#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import shutil
import subprocess
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--cache-dir", default=".competitive-verifier/cache/standalone-assets")
    parser.add_argument("--cxx", default="g++")
    parser.add_argument("--cxxflags", default="-std=c++20 -O2 -Wall -Wextra")
    args = parser.parse_args()

    root = Path(".")
    cache_dir = Path(args.cache_dir)
    tests = sorted((root / "test" / "standalone").glob("*.test.cpp"))

    for test in tests:
        name = test.name[: -len(".test.cpp")]
        generator = root / "test" / "generator" / name / "generator.py"
        checker = root / "test" / "checker" / name / "checker.py"
        if not generator.exists():
            raise RuntimeError(f"missing generator: {generator}")
        if not checker.exists():
            raise RuntimeError(f"missing checker: {checker}")

        case_dir = cache_dir / name / "cases"
        if case_dir.exists():
            shutil.rmtree(case_dir)
        build_dir = cache_dir / name / "build"
        subprocess.run(
            ["python3", str(generator), "--out-dir", str(case_dir)],
            check=True,
        )
        subprocess.run(
            [
                "python3",
                str(checker),
                "--test",
                str(test),
                "--case-dir",
                str(case_dir),
                "--build-dir",
                str(build_dir),
                "--cxx",
                args.cxx,
                "--cxxflags",
                args.cxxflags,
            ],
            check=True,
        )


if __name__ == "__main__":
    main()
