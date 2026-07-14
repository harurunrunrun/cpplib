#!/usr/bin/env python3

from __future__ import annotations

import re
import sys
from pathlib import Path
from urllib.parse import urlsplit


PROBLEM_DIRECTIVES = (
    re.compile(r"competitive-verifier:\s*PROBLEM\s+(https?://[^\s\"']+)"),
    re.compile(r'^\s*#define\s+PROBLEM\s+"(https?://[^\"]+)"', re.MULTILINE),
)


def provider_from_source(source: str) -> str | None:
    problem_url = next(
        (
            match.group(1)
            for pattern in PROBLEM_DIRECTIVES
            if (match := pattern.search(source))
        ),
        None,
    )
    if problem_url is None:
        return None
    hostname = (urlsplit(problem_url).hostname or "").lower()
    for provider, domain in (
        ("atcoder", "atcoder.jp"),
        ("spoj", "spoj.com"),
        ("codechef", "codechef.com"),
    ):
        if hostname == domain or hostname.endswith("." + domain):
            return provider
    return None


def expected_asset_name(wrapper: Path, provider: str) -> str:
    problem = wrapper.name.split(".", 1)[0].lower()
    return f"offline_{provider}_{problem}"


def find_violations(root: Path) -> list[str]:
    onlinejudge = root / "test" / "onlinejudge"
    violations: list[str] = []
    expected_names: dict[str, Path] = {}

    for wrapper in sorted(onlinejudge.glob("*.test.cpp")):
        try:
            source = wrapper.read_text(encoding="utf-8")
        except (OSError, UnicodeError) as error:
            violations.append(f"{wrapper}: could not read: {error}")
            continue
        provider = provider_from_source(source)
        if provider is None:
            continue

        name = expected_asset_name(wrapper, provider)
        previous = expected_names.get(name)
        if previous is not None:
            violations.append(
                f"{wrapper}: offline asset name {name!r} also belongs to {previous}"
            )
            continue
        expected_names[name] = wrapper

        standalone = root / "test" / "standalone" / f"{name}.test.cpp"
        generator_dir = root / "test" / "generator" / name
        checker_dir = root / "test" / "checker" / name

        if not standalone.is_file():
            violations.append(f"{wrapper}: missing standalone wrapper: {standalone}")
        else:
            try:
                standalone_source = standalone.read_text(encoding="utf-8")
            except (OSError, UnicodeError) as error:
                violations.append(f"{standalone}: could not read: {error}")
            else:
                lines = {line.strip() for line in standalone_source.splitlines()}
                if "// competitive-verifier: STANDALONE" not in lines:
                    violations.append(f"{standalone}: missing STANDALONE directive")
                required_include = f'#include "../onlinejudge/{wrapper.name}"'
                if required_include not in lines:
                    violations.append(
                        f"{standalone}: does not include onlinejudge/{wrapper.name}"
                    )

        if not any(
            (generator_dir / filename).is_file()
            for filename in ("generator.py", "generator.cpp")
        ):
            violations.append(f"{wrapper}: missing generator: {generator_dir}")
        if not any(
            (checker_dir / filename).is_file()
            for filename in ("checker.py", "checker.cpp")
        ):
            violations.append(f"{wrapper}: missing checker: {checker_dir}")

    return violations


def main(argv: list[str] | None = None) -> int:
    args = sys.argv[1:] if argv is None else argv
    root = Path(args[0]) if args else Path(".")
    violations = find_violations(root)
    if violations:
        for violation in violations:
            print(violation, file=sys.stderr)
        print(
            f"unsupported onlinejudge asset check failed: {len(violations)} violation(s)",
            file=sys.stderr,
        )
        return 1
    print("unsupported onlinejudge asset check passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
