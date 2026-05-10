#!/usr/bin/env python3

import argparse
import os
import re
import sys
from pathlib import Path
from typing import Optional

INCLUDE_RE = re.compile(r'^(\s*)#\s*include\s*"([^"]+)"(.*)$')
PRAGMA_ONCE_RE = re.compile(r'^\s*#\s*pragma\s+once\s*(?://.*)?$')


def display_path(path: Path) -> str:
    try:
        return os.path.relpath(str(path), str(Path.cwd()))
    except ValueError:
        return str(path)


def find_include(name: str, current_dir: Path, include_dirs: list) -> Path:
    candidates = [current_dir / name]

    for d in include_dirs:
        candidates.append(d / name)

    for p in candidates:
        if p.is_file():
            return p

    raise FileNotFoundError(f'include file not found: {name}')


def expand_lines(
    lines,
    current_dir: Path,
    include_dirs: list,
    out,
    stack: set,
    once_files: set,
    file_key: Optional[Path],
) -> None:
    if file_key is not None and file_key in once_files:
        return

    for line in lines:
        if PRAGMA_ONCE_RE.match(line):
            if file_key is not None:
                once_files.add(file_key)
            continue

        m = INCLUDE_RE.match(line)

        if not m:
            out.write(line)
            continue

        name = m.group(2)
        included = find_include(name, current_dir, include_dirs)
        included_key = included.resolve()

        if included_key in once_files:
            continue

        rel = display_path(included)

        if included_key in stack:
            out.write(f'/* recursive include skipped: {rel} */\n')
            continue

        out.write(f'/* begin include: {rel} */\n')

        stack.add(included_key)
        try:
            with included.open('r', encoding='utf-8') as f:
                expand_lines(
                    f,
                    included.parent,
                    include_dirs,
                    out,
                    stack,
                    once_files,
                    included_key,
                )
        finally:
            stack.remove(included_key)

        out.write(f'/* end include: {rel} */\n')


def main() -> int:
    parser = argparse.ArgumentParser(
        description='Expand only #include "..." from stdin to stdout.'
    )

    parser.add_argument(
        '-I',
        dest='include_dirs',
        action='append',
        default=[],
        help='include directory',
    )

    args = parser.parse_args()

    include_dirs = [Path(d) for d in args.include_dirs]
    current_dir = Path.cwd()

    try:
        expand_lines(
            sys.stdin,
            current_dir,
            include_dirs,
            sys.stdout,
            stack=set(),
            once_files=set(),
            file_key=None,
        )
    except Exception as e:
        print(f'error: {e}', file=sys.stderr)
        return 1

    return 0


if __name__ == '__main__':
    raise SystemExit(main())
