#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path
from urllib.parse import urlsplit


PROBLEM_DIRECTIVES = (
    re.compile(r"competitive-verifier:\s*PROBLEM\s+(https?://[^\s\"']+)"),
    re.compile(r'^\s*#define\s+PROBLEM\s+"(https?://[^\"]+)"', re.MULTILINE),
)
LOCAL_INCLUDE = re.compile(r'^\s*#include\s+"([^\"]+)"', re.MULTILINE)
FRONT_MATTER = re.compile(
    r"\A(?:\ufeff)?---[ \t]*\r?\n(?P<body>.*?)(?=^---[ \t]*\r?$)",
    re.MULTILINE | re.DOTALL,
)
FRONT_MATTER_FIELD = re.compile(
    r"^(?P<key>[A-Za-z_][A-Za-z0-9_-]*):[ \t]*(?P<value>.*?)[ \t]*\r?$",
    re.MULTILINE,
)

TRAILING_TAGS = re.compile(r"(?:\s+\[[^\[\]]+\])+$")
HELPER_SOURCE_STEMS = {
    "common_monoid",
    "common_monoids",
    "common_monoid_with_action",
    "common_monoid_with_action_and_length",
    "fast_modint",
    "fastio",
    "monoid",
    "monoid_act",
    "monoid_act_len",
    "modint",
    "types",
}


PRIMARY_SOURCE_ALIASES = {
    "circle_cross_points": "circle_circle_cross_points",
    # AOJ calls this operation "Point in Polygon", while the public API uses
    # the conventional competitive-programming name ``contains``.
    "point_in_polygon": "contains",
    "segment_cross_point": "cross_point",
}

def problem_url(source: str) -> str | None:
    for pattern in PROBLEM_DIRECTIVES:
        match = pattern.search(source)
        if match is not None:
            return match.group(1)
    return None


def problem_tag(url: str) -> str | None:
    parsed = urlsplit(url)
    hostname = (parsed.hostname or "").lower()
    parts = [part for part in parsed.path.split("/") if part]

    if hostname == "judge.yosupo.jp" or hostname.endswith(".judge.yosupo.jp"):
        marker = "problem"
        transform = lambda value: value
    elif hostname == "atcoder.jp" or hostname.endswith(".atcoder.jp"):
        marker = "tasks"
        transform = str.upper
    elif hostname == "onlinejudge.u-aizu.ac.jp" or hostname.endswith(
        ".onlinejudge.u-aizu.ac.jp"
    ):
        marker = "problems"
        transform = str.upper
    elif hostname == "spoj.com" or hostname.endswith(".spoj.com"):
        marker = "problems"
        transform = str.upper
    else:
        return None

    lowered = [part.lower() for part in parts]
    if marker not in lowered:
        return None
    index = lowered.index(marker) + 1
    if index >= len(parts):
        return None
    return transform(parts[index])


def normalize_identifier(value: str) -> str:
    return "".join(character for character in value.lower() if character.isalnum())


def reachable_sources(direct_targets: list[Path]) -> list[Path]:
    result: list[Path] = []
    seen: set[Path] = set()
    stack = list(reversed(direct_targets))
    while stack:
        target = stack.pop()
        if target in seen:
            continue
        seen.add(target)
        result.append(target)
        try:
            source = target.read_text(encoding="utf-8")
        except (OSError, UnicodeError):
            continue
        nested = [
            (target.parent / include).resolve()
            for include in LOCAL_INCLUDE.findall(source)
        ]
        stack.extend(
            candidate for candidate in reversed(nested) if "src" in candidate.parts
        )
    return result


def primary_sources(wrapper: Path, source: str) -> list[Path]:
    direct_targets = [
        (wrapper.parent / include).resolve()
        for include in LOCAL_INCLUDE.findall(source)
        if "src" in (wrapper.parent / include).resolve().parts
    ]
    if not direct_targets:
        return []
    targets = reachable_sources(direct_targets)

    stem = wrapper.name[: -len(".test.cpp")]
    key = stem.split(".", 1)[-1]
    key = PRIMARY_SOURCE_ALIASES.get(key, key)
    normalized_key = normalize_identifier(key)
    non_helper_targets = [
        target for target in targets if target.stem not in HELPER_SOURCE_STEMS
    ]
    scoring_targets = non_helper_targets or targets
    scores: list[tuple[int, int, int]] = []
    for target in scoring_targets:
        normalized_stem = normalize_identifier(target.stem)
        if normalized_stem == normalized_key:
            score = (3, len(normalized_stem), 0)
        elif len(normalized_key) >= 4 and normalized_key in normalized_stem:
            # Prefer a more-specific header such as
            # circle_circle_cross_points.hpp for ``circle_cross_points``.
            score = (
                2,
                -abs(len(normalized_stem) - len(normalized_key)),
                len(normalized_stem),
            )
        elif len(normalized_stem) >= 4 and normalized_stem in normalized_key:
            # For ``segment_cross_point``, prefer cross_point.hpp to the
            # transitive cross.hpp dependency by using the longest match.
            score = (1, len(normalized_stem), 0)
        else:
            score = (0, 0, 0)
        scores.append(score)
    best = max(scores)
    if best[0] > 0:
        return [
            target
            for target, score in zip(scoring_targets, scores)
            if score == best
        ]

    non_helpers = [target for target in direct_targets if target.stem not in HELPER_SOURCE_STEMS]
    if len(non_helpers) == 1:
        return non_helpers
    return non_helpers or direct_targets


def source_problem_tags(onlinejudge_dir: Path) -> dict[Path, set[str]]:
    result: dict[Path, set[str]] = defaultdict(set)
    for wrapper in sorted(onlinejudge_dir.glob("*.test.cpp")):
        try:
            source = wrapper.read_text(encoding="utf-8")
        except (OSError, UnicodeError):
            continue
        url = problem_url(source)
        tag = problem_tag(url) if url is not None else None
        if tag is None:
            continue
        for target in primary_sources(wrapper, source):
            result[target].add(tag)
    return result


def front_matter_field(
    source: str,
    key: str,
) -> tuple[str, int, int, str] | None:
    front_matter = FRONT_MATTER.search(source)
    if front_matter is None:
        return None
    matches = [
        match
        for match in FRONT_MATTER_FIELD.finditer(front_matter.group("body"))
        if match.group("key") == key
    ]
    if len(matches) != 1:
        return None

    match = matches[0]
    raw_value = match.group("value")
    quote = ""
    value = raw_value
    if (
        len(raw_value) >= 2
        and raw_value[0] == raw_value[-1]
        and raw_value[0] in "\"'"
    ):
        quote = raw_value[0]
        value = raw_value[1:-1]

    offset = front_matter.start("body")
    return (
        value,
        offset + match.start("value"),
        offset + match.end("value"),
        quote,
    )


def encode_scalar(value: str, quote: str) -> str:
    if quote:
        return quote + value + quote
    return value


def desired_title(title: str, tags: set[str]) -> str:
    base = TRAILING_TAGS.sub("", title).rstrip()
    suffix = "".join(f" [{tag}]" for tag in sorted(tags, key=str.casefold))
    return base + suffix


def update_docs(
    docs_dir: Path,
    onlinejudge_dir: Path,
    *,
    write: bool,
) -> list[str]:
    tags_by_source = source_problem_tags(onlinejudge_dir)
    violations: list[str] = []

    for document in sorted(docs_dir.glob("*.md")):
        try:
            source = document.read_text(encoding="utf-8")
        except (OSError, UnicodeError) as error:
            violations.append(f"{document}: could not read: {error}")
            continue
        documentation_field = front_matter_field(source, "documentation_of")
        title_field = front_matter_field(source, "title")
        if documentation_field is None or title_field is None:
            continue
        documentation_value = documentation_field[0]
        title_value, title_start, title_end, title_quote = title_field
        documented_source = (document.parent / documentation_value).resolve()
        tags = tags_by_source.get(documented_source, set())
        expected = desired_title(title_value, tags)
        if title_value == expected:
            continue
        if write:
            encoded = encode_scalar(expected, title_quote)
            updated = source[:title_start] + encoded + source[title_end:]
            document.write_text(updated, encoding="utf-8")
        else:
            violations.append(
                f"{document}: title must be {expected!r}, got {title_value!r}"
            )

    return violations


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("docs", type=Path)
    parser.add_argument("onlinejudge", type=Path)
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args(argv)

    violations = update_docs(args.docs, args.onlinejudge, write=args.write)
    if violations:
        for violation in violations:
            print(violation, file=sys.stderr)
        print(f"docs problem tag check failed: {len(violations)} violation(s)", file=sys.stderr)
        return 1
    action = "updated" if args.write else "passed"
    print(f"docs problem tag check {action}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
