#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


PRAGMA_ONCE = re.compile(r"^\s*#\s*pragma\s+once(?:\s|$)")
FINAL_ENDIF = re.compile(r"^\s*#\s*endif(?:\s|$)")


def repository_relative(path: Path, repository_root: Path) -> Path:
    return path.absolute().relative_to(repository_root.absolute())


def expected_guard(path: Path, repository_root: Path) -> str:
    relative = repository_relative(path, repository_root)
    normalized = re.sub(r"[^A-Za-z0-9]", "_", relative.as_posix()).upper()
    return f"CPPLIB_{normalized}_INCLUDED"


def header_paths(inputs: list[Path]) -> list[Path]:
    result: set[Path] = set()
    for path in inputs:
        if path.is_dir():
            result.update(
                candidate
                for candidate in path.rglob("*.hpp")
                if candidate.is_file()
            )
        elif path.suffix == ".hpp" and path.is_file():
            result.add(path)
    return sorted(result)


def converted_text(
    path: Path,
    text: str,
    repository_root: Path,
) -> str:
    lines = text.splitlines()
    guard = expected_guard(path, repository_root)
    if lines and PRAGMA_ONCE.match(lines[0]):
        body = lines[1:]
    elif (
        len(lines) >= 3
        and lines[0].startswith("#ifndef ")
        and lines[1].startswith("#define ")
    ):
        last = len(lines) - 1
        while last >= 0 and not lines[last]:
            last -= 1
        if last < 2 or not FINAL_ENDIF.match(lines[last]):
            raise ValueError("existing include guard has no final #endif")
        body = lines[2:last]
    else:
        raise ValueError(
            "header must start with #pragma once or an include guard"
        )
    while body and not body[-1]:
        body.pop()
    return "\n".join([
        f"#ifndef {guard}",
        f"#define {guard}",
        *body,
        "",
        f"#endif  // {guard}",
        "",
    ])


def text_violations(
    texts: list[tuple[Path, str]],
    repository_root: Path,
) -> list[str]:
    result: list[str] = []
    guards: dict[str, Path] = {}
    for path, text in texts:
        lines = text.splitlines()
        guard = expected_guard(path, repository_root)
        if any(PRAGMA_ONCE.match(line) for line in lines):
            result.append(f"{path}: #pragma once is forbidden")
        if len(lines) < 3 or lines[0] != f"#ifndef {guard}":
            result.append(
                f"{path}: first line must be '#ifndef {guard}'"
            )
        if len(lines) < 3 or lines[1] != f"#define {guard}":
            result.append(
                f"{path}: second line must be '#define {guard}'"
            )
        last = len(lines) - 1
        while last >= 0 and not lines[last]:
            last -= 1
        expected_end = f"#endif  // {guard}"
        if last < 0 or lines[last] != expected_end:
            result.append(
                f"{path}: last nonempty line must be '{expected_end}'"
            )
        previous = guards.get(guard)
        if previous is not None:
            result.append(
                f"{path}: include guard duplicates {previous}: {guard}"
            )
        guards[guard] = path
    return result


def violations(
    paths: list[Path],
    repository_root: Path,
) -> list[str]:
    return text_violations([
        (path, path.read_text(encoding="utf-8")) for path in paths
    ], repository_root)


def has_symlink_component(path: Path, repository_root: Path) -> bool:
    try:
        relative = repository_relative(path, repository_root)
    except ValueError:
        return True
    current = repository_root.absolute()
    for part in relative.parts:
        current /= part
        if current.is_symlink():
            return True
    return False


def run(
    inputs: list[Path],
    repository_root: Path,
    fix: bool,
) -> int:
    missing = [path for path in inputs if not path.exists()]
    if missing:
        for path in missing:
            print(f"{path}: input path does not exist", file=sys.stderr)
        return 2
    paths = header_paths(inputs)
    if not paths:
        print("header guard check failed: no .hpp files found", file=sys.stderr)
        return 2
    unsafe = [
        path for path in paths
        if has_symlink_component(path, repository_root)
    ]
    if unsafe:
        for path in unsafe:
            print(
                f"{path}: symlink or path outside repository is forbidden",
                file=sys.stderr,
            )
        return 2
    if fix:
        prepared: list[tuple[Path, str, str]] = []
        for path in paths:
            original = path.read_text(encoding="utf-8")
            try:
                converted = converted_text(path, original, repository_root)
            except ValueError as error:
                print(f"{path}: {error}", file=sys.stderr)
                return 2
            prepared.append((path, original, converted))
        errors = text_violations([
            (path, converted) for path, _, converted in prepared
        ], repository_root)
        if errors:
            for error in errors:
                print(error, file=sys.stderr)
            print(
                "header guard fix refused before writing: "
                f"{len(errors)} violation(s)",
                file=sys.stderr,
            )
            return 2
        changed_during_preflight = [
            path for path, original, _ in prepared
            if path.read_text(encoding="utf-8") != original
        ]
        if changed_during_preflight:
            for path in changed_during_preflight:
                print(
                    f"{path}: changed during header guard preflight",
                    file=sys.stderr,
                )
            return 2
        for path, original, converted in prepared:
            if converted != original:
                path.write_text(converted, encoding="utf-8")
    errors = violations(paths, repository_root)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        print(
            f"header guard check failed: {len(errors)} violation(s)",
            file=sys.stderr,
        )
        return 1
    print(f"header guard check passed: {len(paths)} header(s)")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--fix", action="store_true")
    parser.add_argument(
        "paths", nargs="*", type=Path, default=[Path("src"), Path("test")]
    )
    args = parser.parse_args()
    repository_root = Path.cwd()
    return run(args.paths, repository_root, args.fix)


if __name__ == "__main__":
    raise SystemExit(main())
