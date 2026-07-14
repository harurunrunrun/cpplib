#!/usr/bin/env python3
"""Check the command-line contract of standalone generators."""

from __future__ import annotations

import argparse
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class InterfaceFailure:
    path: Path
    message: str


def discover_generators(root: Path) -> list[Path]:
    generators = []
    for test in sorted((root / "test" / "standalone").glob("*.test.cpp")):
        name = test.name[: -len(".test.cpp")]
        generator = root / "test" / "generator" / name / "generator.py"
        if generator.is_file():
            generators.append(generator)
    return generators


def discover_cpp_generators(root: Path) -> list[Path]:
    generators = []
    for test in sorted((root / "test" / "standalone").glob("*.test.cpp")):
        name = test.name[: -len(".test.cpp")]
        generator = root / "test" / "generator" / name / "generator.cpp"
        if generator.is_file():
            generators.append(generator)
    return generators


def check_generator(
    generator: Path,
    *,
    root: Path,
    timeout: float,
) -> InterfaceFailure | None:
    try:
        completed = subprocess.run(
            [sys.executable, str(generator), "--help"],
            cwd=root,
            check=False,
            capture_output=True,
            text=True,
            timeout=timeout,
        )
    except (OSError, subprocess.TimeoutExpired) as error:
        return InterfaceFailure(generator, f"--help could not complete: {error}")

    help_text = completed.stdout + completed.stderr
    if completed.returncode != 0:
        return InterfaceFailure(
            generator,
            f"--help exited with code {completed.returncode}",
        )
    if "--out-dir" not in help_text:
        return InterfaceFailure(generator, "--help does not advertise --out-dir")
    return None


def check_cpp_generator(generator: Path) -> InterfaceFailure | None:
    """Statically check native generators, which do not implement --help."""

    try:
        source = generator.read_text(encoding="utf-8")
    except (OSError, UnicodeError) as error:
        return InterfaceFailure(generator, f"source could not be read: {error}")
    if '"--out-dir"' not in source:
        return InterfaceFailure(
            generator,
            "native generator source does not advertise --out-dir",
        )
    return None


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("root", nargs="?", type=Path, default=Path("."))
    parser.add_argument("--jobs", type=int, default=8)
    parser.add_argument("--timeout", type=float, default=10.0)
    args = parser.parse_args(argv)
    if args.jobs <= 0:
        parser.error("--jobs must be positive")
    if args.timeout <= 0:
        parser.error("--timeout must be positive")

    root = args.root.resolve()
    generators = discover_generators(root)
    cpp_generators = discover_cpp_generators(root)
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        failures = [
            failure
            for failure in executor.map(
                lambda generator: check_generator(
                    generator,
                    root=root,
                    timeout=args.timeout,
                ),
                generators,
            )
            if failure is not None
        ]
    failures.extend(
        failure
        for failure in map(check_cpp_generator, cpp_generators)
        if failure is not None
    )

    if failures:
        for failure in failures:
            print(
                f"{failure.path.relative_to(root)}: {failure.message}",
                file=sys.stderr,
            )
        print(
            f"standalone generator interface check failed: {len(failures)} "
            f"violation(s), {len(generators)} Python and "
            f"{len(cpp_generators)} C++ generator(s)",
            file=sys.stderr,
        )
        return 1
    print(
        f"standalone generator interface check passed: "
        f"{len(generators)} Python and {len(cpp_generators)} C++ generator(s)",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
