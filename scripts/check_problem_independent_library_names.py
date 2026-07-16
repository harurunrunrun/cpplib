#!/usr/bin/env python3
"""Reject online-judge stories and brands from public library names and docs."""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path


# These identifiers were once public answer functions or adapters. Keeping the
# list explicit makes a renamed problem-specific API fail before it can become
# part of the documented interface again.
OLD_PUBLIC_IDENTIFIERS = (
    "bottom_vertices",
    "bug_interactions_are_consistent",
    "can_feed_all_cats",
    "capital_candidate_vertices",
    "card_trick_permutation",
    "critical_edges",
    "count_false_food_chain_statements",
    "count_morse_decodings",
    "cow_game",
    "employee_salary_tree",
    "enterprise_network",
    "exactly_once_guard_coverage",
    "find_word_puzzle_placements",
    "fraction_tree_nth",
    "gcd_extreme",
    "gondor_spark_lighting_times",
    "grass_planting_tree",
    "integer_factorization_15_digits",
    "letter_pipe_max_flow",
    "lucifer_number_count",
    "maximum_batman_cliff_count",
    "maximum_commando_effectiveness",
    "maximum_fun_walk",
    "maximum_labyrinth_rope_length",
    "maximum_non_crossing_bridges",
    "maximum_nonseeing_fighters",
    "maximum_party_fun",
    "maximum_philosopher_stones",
    "maximum_potholer_count",
    "maximum_route_success_percent",
    "maximum_simultaneous_guests",
    "maximum_taxi_pickups",
    "maximum_treat_revenue",
    "minimum_exact_apple_cost",
    "minimum_fishmonger_toll",
    "minimum_foxling_crackers",
    "minimum_herding_traps",
    "minimum_land_acquisition_cost",
    "minimum_leftward_leaf_gathering_cost",
    "minimum_lsort_cost",
    "minimum_mixture_smoke",
    "minimum_network_connection_cost",
    "minimum_opinion_disagreement",
    "minimum_optimal_tree_ttl",
    "minimum_paved_street_cost",
    "minimum_pilot_salary",
    "minimum_publicity_contacts",
    "minimum_scuba_cylinder_weight",
    "minimum_string_break_cost",
    "minimum_temporary_staff_cost",
    "neighboring_town_pairs",
    "offline_road_disconnection",
    "penguin_forest",
    "perfect_rhyme_dictionary",
    "phone_list_consistency",
    "qtree_edge_maximum",
    "qtree_distance_kth_vertex",
    "sorted_queue_lengths",
    "tree_isomorphism_answer",
    "two_disjoint_tree_paths_answer",
    "weighted_grid_escape",
    "words_can_form_chain",
    "xor_decoded_range_greater_query",
)

# Problem codes may identify verification sources only in the trailing portion
# of a documentation title. They must not leak into an API, filename, or prose.
CASE_INSENSITIVE_PROBLEM_CODES = (
    "BAT3", "BUGLIFE", "CHICAGO", "COCONUTS", "CSTREET",
    "HERDING", "KQUERYO", "LSORT", "MKBUDGET", "MPILOT", "NITTROAD",
    "NKLEAVES", "OTOCI", "QTREE", "QTREE2", "ROADNET", "SHPATH",
    "TAXI", "TEMPLEQ", "TWOPATHS",
)

# acquire is a normal library verb, so only the uppercase verification code is
# reserved. The other codes are distinctive enough to reject in lowercase
# public paths and identifiers too.
CASE_SENSITIVE_PROBLEM_CODES = ("ACQUIRE", "BOTTOM")
FISHER_PROBLEM_CODES = ("FISHER",)
PROBLEM_CODES = (
    CASE_SENSITIVE_PROBLEM_CODES
    + CASE_INSENSITIVE_PROBLEM_CODES
    + FISHER_PROBLEM_CODES
)

TRAILING_VERIFICATION_TAGS = re.compile(r"(?:\s+\[[^\[\]\n]+\])+\s*$")
TITLE_FIELD = re.compile(r"^title:[ \t]*(?P<value>.*?)[ \t]*$")


@dataclass(frozen=True)
class Rule:
    name: str
    pattern: re.Pattern[str]
    explanation: str


@dataclass(frozen=True)
class Violation:
    path: Path
    line: int | None
    context: str
    rule: str
    term: str
    explanation: str

    def format(self) -> str:
        location = str(self.path)
        if self.line is not None:
            location += f":{self.line}"
        return (
            f"{location}: {self.context} contains {self.term!r} "
            f"({self.rule}: {self.explanation})"
        )


def alternatives(values: tuple[str, ...]) -> str:
    return "|".join(re.escape(value) for value in sorted(values, key=len, reverse=True))


RULES = (
    Rule(
        "online-judge-reference",
        re.compile(
            r"https?://[^\s\"')>]*(?:"
            r"spoj\.com|atcoder\.jp|judge\.yosupo\.jp|"
            r"onlinejudge\.u-aizu\.ac\.jp"
            r")[^\s\"')>]*"
            r"|(?<![A-Za-z0-9])(?:SPOJ|AtCoder|AOJ|Aizu Online Judge|"
            r"Yosupo(?: Judge)?|Library Checker)"
            r"(?![A-Za-z0-9])",
            re.IGNORECASE,
        ),
        "verification services belong in tests, not the public library",
    ),
    Rule(
        "retired-problem-api",
        re.compile(alternatives(OLD_PUBLIC_IDENTIFIERS), re.IGNORECASE),
        "use the generic operation or data-structure name",
    ),
    Rule(
        "problem-code",
        re.compile(
            r"(?<![A-Za-z0-9])(?:"
            + alternatives(CASE_INSENSITIVE_PROBLEM_CODES)
            + r")(?![A-Za-z0-9])",
            re.IGNORECASE,
        ),
        "problem codes are allowed only as trailing documentation title tags",
    ),
    Rule(
        "problem-code",
        re.compile(
            r"(?<![A-Za-z0-9])(?:FISHER)"
            r"(?!(?:[-_ –—]?Yates))"
            r"(?![A-Za-z0-9])",
            re.IGNORECASE,
        ),
        "problem codes are allowed only as trailing documentation title tags",
    ),
    Rule(
        "problem-code",
        re.compile(
            r"(?<![A-Za-z0-9])(?:"
            + alternatives(CASE_SENSITIVE_PROBLEM_CODES)
            + r")(?![A-Za-z0-9])"
        ),
        "problem codes are allowed only as trailing documentation title tags",
    ),
    Rule(
        "problem-story",
        re.compile(
            r"(?<![A-Za-z])(?:Batman|Bottom Vertices|Commando|Critical Edges|Fisher(?!(?:[-_ –—]?Yates))|Fishmonger|Pilot|Taxi|Penguins?|Herding|"
            r"Coconuts?|Temple Queues?|Bug Interactions?|Cows?|"
            r"Feed (?:All )?Cats|Employee Salary|Enterprise Network|"
            r"Gondor Sparks?|Grass Planting|Lucifer Numbers?|"
            r"Labyrinth Rope|Nonseeing Fighters?|Party Fun|"
            r"Philosopher Stones?|Potholers?|Simultaneous Guests?|"
            r"Treat Revenue|Exact Apple Cost|Foxling Crackers?|"
            r"Mixture Smoke|Optimal Tree TTL|Publicity Contacts?|"
            r"Scuba Cylinders?|Rhyme Dictionary|Phone List|Grid Escape|"
            r"Route Success Percent(?:age)?|Temporary Staff|Leaf Gathering|"
            r"Opinion Disagreement|Paved Streets?|Neighboring Towns?|"
            r"Road Disconnection|Land Acquisition)(?![A-Za-z])"
            r"|虫の相互作用|バットマン|司令官|魚屋|漁師|操縦士|"
            r"タクシー|ペンギン|牧畜|寺院のキュー|崖登り|葉集め|道路切断|"
            r"有向グラフの底頂点|切断辺列挙",
            re.IGNORECASE,
        ),
        "describe the general graph, sequence, optimization, or data-structure operation",
    ),
)


def decoded_title_value(raw_value: str) -> str:
    """Return enough of a YAML scalar to check its human-readable title."""

    value = raw_value.strip()
    if len(value) >= 2 and value[0] == value[-1] and value[0] in "\"'":
        value = value[1:-1]
    return value


def generic_title(raw_value: str) -> str:
    """Remove only valid trailing verification tags from a title scalar."""

    return TRAILING_VERIFICATION_TAGS.sub("", decoded_title_value(raw_value))


def matches(
    path: Path,
    line: int | None,
    context: str,
    text: str,
) -> list[Violation]:
    result: list[Violation] = []
    for rule in RULES:
        for match in rule.pattern.finditer(text):
            result.append(
                Violation(
                    path, line, context, rule.name, match.group(0), rule.explanation
                )
            )
    return result


def check_file(path: Path, *, is_documentation: bool | None = None) -> list[Violation]:
    """Return problem-dependent public-name violations in one header or doc."""

    if is_documentation is None:
        is_documentation = path.suffix.lower() == ".md"

    violations = matches(path, None, "path", path.as_posix())
    try:
        lines = path.read_text(encoding="utf-8-sig").splitlines()
    except (OSError, UnicodeError) as error:
        violations.append(
            Violation(
                path, None, "file", "utf-8", str(error),
                "public files must be readable as UTF-8",
            )
        )
        return violations

    in_front_matter = bool(
        is_documentation and lines and lines[0].strip() == "---"
    )
    for index, line in enumerate(lines, start=1):
        if is_documentation and in_front_matter:
            if index > 1 and line.strip() == "---":
                in_front_matter = False
                continue
            title = TITLE_FIELD.match(line)
            if title is not None:
                violations.extend(
                    matches(
                        path,
                        index,
                        "documentation title",
                        generic_title(title.group("value")),
                    )
                )
                continue

        context = "documentation" if is_documentation else "public source"
        violations.extend(matches(path, index, context, line))

    return violations


def public_files(src_root: Path, docs_root: Path) -> tuple[list[Path], list[Path]]:
    """Collect the exact public surface: recursive headers and flat docs."""

    headers = sorted(
        (path for path in src_root.rglob("*.hpp") if path.is_file()),
        key=lambda path: path.as_posix(),
    )
    documents = sorted(
        (path for path in docs_root.glob("*.md") if path.is_file()),
        key=lambda path: path.as_posix(),
    )
    return headers, documents


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "reject online-judge brands, stories, and retired problem APIs from "
            "public headers and documentation"
        )
    )
    parser.add_argument(
        "src_root", nargs="?", type=Path, default=Path("src"),
        help="recursive public header root (default: src)",
    )
    parser.add_argument(
        "docs_root", nargs="?", type=Path, default=Path("docs"),
        help="flat documentation directory (default: docs)",
    )
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    headers, documents = public_files(arguments.src_root, arguments.docs_root)
    violations = [
        violation
        for path in headers
        for violation in check_file(path, is_documentation=False)
    ]
    violations.extend(
        violation
        for path in documents
        for violation in check_file(path, is_documentation=True)
    )

    if violations:
        print(*(violation.format() for violation in violations), sep="\n", file=sys.stderr)
        print(
            "problem-independent library name check failed: "
            f"{len(violations)} violation(s), {len(headers)} header(s), "
            f"{len(documents)} doc file(s)",
            file=sys.stderr,
        )
        return 1

    print(
        "problem-independent library name check passed: "
        f"{len(headers)} header(s), {len(documents)} doc file(s)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
