#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import re
from pathlib import Path


def prerequisites(makefile: str, target: str) -> set[str]:
    match = re.search(
        rf"(?m)^{re.escape(target)}\s*:\s*([^\n]*)$",
        makefile,
    )
    if match is None:
        raise AssertionError(f"missing Makefile target: {target}")
    return set(match.group(1).split())


def main() -> None:
    makefile = Path("Makefile").read_text(encoding="utf-8")
    verifier_dependencies = prerequisites(makefile, "verifier-resolve")
    docs_dependencies = prerequisites(makefile, "docs-verifier-resolve")

    compiler_checks = {"gcc13-check", "verifier-wrapper-test"}
    missing = compiler_checks - verifier_dependencies
    if missing:
        raise AssertionError(
            "verify compiler enforcement is missing: "
            + ", ".join(sorted(missing))
        )

    forbidden = compiler_checks & docs_dependencies
    if forbidden:
        raise AssertionError(
            "docs-only resolve must not require a compiler: "
            + ", ".join(sorted(forbidden))
        )

    if "verifier-setup" not in docs_dependencies:
        raise AssertionError("docs-only resolve still requires verifier-setup")

    docs_recipe = re.search(
        r"(?ms)^docs-verifier-resolve:[^\n]*\n((?:\t[^\n]*\n)+)",
        makefile,
    )
    if docs_recipe is None:
        raise AssertionError("missing docs-verifier-resolve recipe")
    if "$(VERIFIER_COMMAND_ENV)" in docs_recipe.group(1):
        raise AssertionError(
            "docs-only resolve must use the local default compiler"
        )


if __name__ == "__main__":
    main()
