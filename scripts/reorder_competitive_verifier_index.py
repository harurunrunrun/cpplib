#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import html
import re
from pathlib import Path
from typing import Any

import yaml


SECTION_ORDER = ("algorithm", "structure", "integer_geometry", "approximate")
VERIFICATION_PATH_PREFIXES = ("test/onlinejudge/", "test/standalone/")
VERIFICATION_CATEGORY_ORDER = VERIFICATION_PATH_PREFIXES
CONTENTS_START = "<!-- competitive-verifier-folder-contents:start -->"
CONTENTS_END = "<!-- competitive-verifier-folder-contents:end -->"
ORIGINAL_CATEGORY_HEADING = "<h3>{{ category.name }}</h3>"
ANCHORED_CATEGORY_HEADING = (
    '<h3{% if category.anchor %} id="{{ category.anchor | xml_escape }}"'
    "{% endif %}>{{ category.name }}</h3>"
)


def split_front_matter(text: str) -> tuple[dict[str, Any], str]:
    lines = text.splitlines(keepends=True)
    if not lines or lines[0].strip() != "---":
        raise ValueError("front matter opening delimiter is missing")

    for index in range(1, len(lines)):
        if lines[index].strip() == "---":
            data = yaml.safe_load("".join(lines[1:index]))
            if not isinstance(data, dict):
                raise ValueError("front matter must be a mapping")
            return data, "".join(lines[index + 1 :])
    raise ValueError("front matter closing delimiter is missing")


def section_name(category: Any) -> str | None:
    if not isinstance(category, dict):
        return None
    name = category.get("name")
    if not isinstance(name, str):
        return None
    components = name.strip("/").split("/")
    if len(components) < 2 or components[0] != "src":
        return None
    return components[1]


def page_path(page: Any) -> str | None:
    if not isinstance(page, dict):
        return None
    path = page.get("path")
    return path if isinstance(path, str) else None


def entry_categories(entry: dict[str, Any]) -> list[Any]:
    categories = entry.get("categories")
    if not isinstance(categories, list):
        raise ValueError(f"{entry.get('type', 'index entry')} does not contain categories")
    return categories


def category_pages(category: dict[str, Any]) -> list[Any]:
    pages = category.get("pages")
    if not isinstance(pages, list):
        raise ValueError("index category does not contain pages")
    return pages


def category_anchor(name: str) -> str:
    slug = re.sub(r"[^0-9A-Za-z]+", "-", name.strip("/")).strip("-").lower()
    digest = hashlib.sha1(name.encode("utf-8")).hexdigest()[:8]
    return f"library-directory-{slug}-{digest}"


def add_library_category_anchors(library_entry: dict[str, Any]) -> bool:
    changed = False
    used: set[str] = set()
    for category in entry_categories(library_entry):
        if not isinstance(category, dict):
            continue
        name = category.get("name")
        if not isinstance(name, str):
            continue
        anchor = category_anchor(name)
        if anchor in used:
            raise ValueError(f"duplicate library category anchor: {anchor}")
        used.add(anchor)
        if category.get("anchor") != anchor:
            category["anchor"] = anchor
            changed = True
    return changed


def library_folder_tree(library_entry: dict[str, Any]) -> dict[str, Any]:
    tree: dict[str, Any] = {}
    for category in entry_categories(library_entry):
        if not isinstance(category, dict):
            continue
        name = category.get("name")
        anchor = category.get("anchor")
        if not isinstance(name, str) or not isinstance(anchor, str):
            continue
        components = [component for component in name.strip("/").split("/") if component]
        if not components:
            continue
        nodes = tree
        ancestry: list[dict[str, Any]] = []
        for component in components:
            node = nodes.setdefault(
                component,
                {"children": {}, "anchor": None, "first_anchor": None},
            )
            ancestry.append(node)
            nodes = node["children"]
        node["anchor"] = anchor
        for ancestor in ancestry:
            if ancestor["first_anchor"] is None:
                ancestor["first_anchor"] = anchor
    return tree


def render_folder_tree(nodes: dict[str, Any], indent: str = "") -> list[str]:
    lines = [f'{indent}<ul class="library-folder-tree">']
    for name, node in nodes.items():
        anchor = node["first_anchor"]
        if not isinstance(anchor, str):
            continue
        lines.append(
            f'{indent}  <li><a href="#{html.escape(anchor, quote=True)}">'
            f"{html.escape(name)}/</a>"
        )
        if node["children"]:
            lines.extend(render_folder_tree(node["children"], indent + "    "))
        lines.append(f"{indent}  </li>")
    lines.append(f"{indent}</ul>")
    return lines


def contents_block(front_matter: dict[str, Any]) -> str:
    data = front_matter.get("data")
    top = data.get("top") if isinstance(data, dict) else None
    if not isinstance(top, list):
        raise ValueError("front matter data does not contain top")
    tree = library_folder_tree(find_entry(top, "Library Files"))
    lines = [
        CONTENTS_START,
        '<h2 id="contents">Contents</h2>',
        '<nav aria-labelledby="contents">',
        *render_folder_tree(tree),
        "</nav>",
        CONTENTS_END,
    ]
    return "\n".join(lines)


def normalize_content(front_matter: dict[str, Any], content: str) -> str:
    marker_pattern = re.compile(
        rf"\n*{re.escape(CONTENTS_START)}.*?{re.escape(CONTENTS_END)}\n*",
        re.DOTALL,
    )
    base = marker_pattern.sub("\n", content).rstrip()
    block = contents_block(front_matter)
    spacer = base.rfind("<br>")
    if spacer == -1:
        return f"{base}\n\n{block}\n"
    return f"{base[:spacer].rstrip()}\n\n{block}\n\n{base[spacer:]}\n"


def normalize_template(template: str) -> tuple[str, bool]:
    if ANCHORED_CATEGORY_HEADING in template:
        return template, False
    if ORIGINAL_CATEGORY_HEADING not in template:
        raise ValueError("competitive-verifier category heading template is unsupported")
    return template.replace(
        ORIGINAL_CATEGORY_HEADING,
        ANCHORED_CATEGORY_HEADING,
        1,
    ), True


def find_entry(top: list[Any], entry_type: str) -> dict[str, Any]:
    matches = [
        entry
        for entry in top
        if isinstance(entry, dict) and entry.get("type") == entry_type
    ]
    if len(matches) != 1:
        raise ValueError(f"exactly one {entry_type} entry is required")
    return matches[0]


def merged_pages(existing: list[Any], moved: list[Any]) -> list[Any]:
    by_path: dict[str, Any] = {}
    without_path: list[Any] = []
    for page in existing + moved:
        path = page_path(page)
        if path is None:
            without_path.append(page)
        elif path not in by_path:
            by_path[path] = page
    return [by_path[path] for path in sorted(by_path)] + without_path


def move_verification_pages(
    library_entry: dict[str, Any],
    verification_entry: dict[str, Any],
) -> bool:
    library_categories = entry_categories(library_entry)
    verification_categories = entry_categories(verification_entry)
    verification_by_name = {
        category.get("name"): category
        for category in verification_categories
        if isinstance(category, dict) and isinstance(category.get("name"), str)
    }

    changed = False
    kept_categories: list[Any] = []
    for category in library_categories:
        if not isinstance(category, dict):
            kept_categories.append(category)
            continue

        kept_pages: list[Any] = []
        moved_pages: list[Any] = []
        for page in category_pages(category):
            path = page_path(page)
            if path is not None and path.startswith(VERIFICATION_PATH_PREFIXES):
                moved_pages.append(page)
            else:
                kept_pages.append(page)

        if not moved_pages:
            kept_categories.append(category)
            continue

        changed = True
        if kept_pages:
            category["pages"] = kept_pages
            kept_categories.append(category)

        moved_category = dict(category)
        moved_category["pages"] = merged_pages([], moved_pages)
        name = moved_category.get("name")
        destination = verification_by_name.get(name)
        if destination is None:
            verification_categories.append(moved_category)
            if isinstance(name, str):
                verification_by_name[name] = moved_category
        else:
            destination["pages"] = merged_pages(
                category_pages(destination), moved_pages
            )

    if changed:
        library_entry["categories"] = kept_categories
    return changed


def normalize_index(front_matter: dict[str, Any]) -> bool:
    data = front_matter.get("data")
    if not isinstance(data, dict):
        raise ValueError("front matter does not contain data")
    top = data.get("top")
    if not isinstance(top, list):
        raise ValueError("front matter data does not contain top")

    library_entry = find_entry(top, "Library Files")
    verification_entry = find_entry(top, "Verification Files")
    changed = move_verification_pages(library_entry, verification_entry)

    verification_categories = entry_categories(verification_entry)
    verification_rank = {
        name: index for index, name in enumerate(VERIFICATION_CATEGORY_ORDER)
    }
    verification_positions = {
        id(category): index for index, category in enumerate(verification_categories)
    }
    reordered_verification = sorted(
        verification_categories,
        key=lambda category: (
            verification_rank.get(
                category.get("name") if isinstance(category, dict) else None,
                len(verification_rank),
            ),
            verification_positions[id(category)],
        ),
    )
    if reordered_verification != verification_categories:
        verification_entry["categories"] = reordered_verification
        changed = True

    categories = entry_categories(library_entry)
    rank = {name: index for index, name in enumerate(SECTION_ORDER)}
    original_positions = {
        id(category): index for index, category in enumerate(categories)
    }
    reordered = sorted(
        categories,
        key=lambda category: (
            rank.get(section_name(category), len(rank)),
            original_positions[id(category)],
        ),
    )
    if reordered != categories:
        library_entry["categories"] = reordered
        changed = True
    if add_library_category_anchors(library_entry):
        changed = True

    library_position = top.index(library_entry)
    verification_position = top.index(verification_entry)
    if verification_position < library_position:
        top.pop(library_position)
        verification_position = top.index(verification_entry)
        top.insert(verification_position, library_entry)
        changed = True
    return changed


def validate_index(front_matter: dict[str, Any]) -> None:
    data = front_matter.get("data")
    top = data.get("top") if isinstance(data, dict) else None
    if not isinstance(top, list):
        raise ValueError("front matter data does not contain top")

    library_entry = find_entry(top, "Library Files")
    verification_entry = find_entry(top, "Verification Files")
    if top.index(library_entry) >= top.index(verification_entry):
        raise ValueError("Verification Files must follow Library Files")

    rank = {name: index for index, name in enumerate(SECTION_ORDER)}
    previous_rank = -1
    anchors: set[str] = set()
    for category in entry_categories(library_entry):
        current_rank = rank.get(section_name(category), len(rank))
        if current_rank < previous_rank:
            raise ValueError("Library Files categories are out of order")
        previous_rank = current_rank
        if not isinstance(category, dict):
            continue
        name = category.get("name")
        anchor = category.get("anchor")
        if not isinstance(name, str) or anchor != category_anchor(name):
            raise ValueError("Library Files category anchor is missing or invalid")
        if anchor in anchors:
            raise ValueError("Library Files category anchors are duplicated")
        anchors.add(anchor)

    for entry in top:
        if entry is verification_entry or not isinstance(entry, dict):
            continue
        categories = entry.get("categories")
        if categories is None:
            continue
        if not isinstance(categories, list):
            raise ValueError(f"{entry.get('type', 'index entry')} does not contain categories")
        for category in categories:
            if not isinstance(category, dict):
                continue
            for page in category_pages(category):
                path = page_path(page)
                if path is not None and path.startswith(VERIFICATION_PATH_PREFIXES):
                    raise ValueError(
                        f"verification file is outside Verification Files: {path}"
                    )

    verification_rank = {
        name: index for index, name in enumerate(VERIFICATION_CATEGORY_ORDER)
    }
    previous_verification_rank = -1
    for category in entry_categories(verification_entry):
        name = category.get("name") if isinstance(category, dict) else None
        current_rank = verification_rank.get(name, len(verification_rank))
        if current_rank < previous_verification_rank:
            raise ValueError("Verification Files categories are out of order")
        previous_verification_rank = current_rank


def dump_front_matter(front_matter: dict[str, Any], content: str) -> str:
    body = yaml.safe_dump(
        front_matter,
        allow_unicode=True,
        sort_keys=False,
        width=10_000,
    )
    return f"---\n{body}---\n{content}"


def main() -> None:
    parser = argparse.ArgumentParser(
        description=(
            "Order competitive-verifier library categories and keep online-judge "
            "and standalone tests under Verification Files."
        )
    )
    parser.add_argument("index", type=Path)
    parser.add_argument(
        "--template",
        type=Path,
        required=True,
        help="competitive-verifier toppage_body.html to add category anchors to",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail instead of rewriting when the index is not normalized",
    )
    args = parser.parse_args()

    text = args.index.read_text(encoding="utf-8")
    front_matter, content = split_front_matter(text)
    changed = normalize_index(front_matter)
    validate_index(front_matter)
    normalized_content = normalize_content(front_matter, content)
    content_changed = normalized_content != content
    template = args.template.read_text(encoding="utf-8")
    normalized_template, template_changed = normalize_template(template)
    if args.check:
        if changed or content_changed or template_changed:
            parser.error("competitive-verifier index is not normalized")
        return

    if changed or content_changed:
        args.index.write_text(
            dump_front_matter(front_matter, normalized_content),
            encoding="utf-8",
        )

    if template_changed:
        args.template.write_text(normalized_template, encoding="utf-8")


if __name__ == "__main__":
    main()
