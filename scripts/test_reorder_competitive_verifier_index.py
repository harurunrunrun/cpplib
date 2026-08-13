#!/usr/bin/env python3

from __future__ import annotations

import unittest

import reorder_competitive_verifier_index as index_order


def page(path: str, icon: str) -> dict[str, str]:
    return {"path": path, "icon": icon}


class NormalizeIndexTest(unittest.TestCase):
    def test_moves_all_onlinejudge_pages_and_preserves_results(self) -> None:
        verified = page("test/onlinejudge/verified.test.cpp", "heavy_check_mark")
        misplaced_verified = page("test/onlinejudge/verified.test.cpp", "warning")
        unsupported = page("test/onlinejudge/unsupported.test.cpp", "warning")
        library_page = page("src/algorithm/example.hpp", "warning")
        front_matter = {
            "data": {
                "top": [
                    {
                        "type": "Verification Files",
                        "categories": [
                            {"name": "test/onlinejudge/", "pages": [verified]}
                        ],
                    },
                    {
                        "type": "Library Files",
                        "categories": [
                            {"name": "src/structure/", "pages": []},
                            {"name": "src/approximate/", "pages": []},
                            {
                                "name": "src/integer_geometry/",
                                "pages": [],
                            },
                            {"name": "src/algorithm/", "pages": [library_page]},
                            {
                                "name": "test/onlinejudge/",
                                "pages": [unsupported, misplaced_verified],
                            },
                        ],
                    },
                ]
            }
        }

        self.assertTrue(index_order.normalize_index(front_matter))
        index_order.validate_index(front_matter)

        top = front_matter["data"]["top"]
        self.assertEqual(
            [entry["type"] for entry in top],
            ["Library Files", "Verification Files"],
        )
        self.assertEqual(
            [category["name"] for category in top[0]["categories"]],
            [
                "src/algorithm/",
                "src/integer_geometry/",
                "src/structure/",
                "src/approximate/",
            ],
        )
        verification_pages = top[1]["categories"][0]["pages"]
        self.assertEqual(
            [entry["path"] for entry in verification_pages],
            [
                "test/onlinejudge/unsupported.test.cpp",
                "test/onlinejudge/verified.test.cpp",
            ],
        )
        self.assertIs(verification_pages[1], verified)

        self.assertFalse(index_order.normalize_index(front_matter))
        index_order.validate_index(front_matter)

    def test_moves_standalone_pages_and_orders_verification_categories(self) -> None:
        standalone = page("test/standalone/example.test.cpp", "warning")
        onlinejudge = page("test/onlinejudge/example.test.cpp", "heavy_check_mark")
        front_matter = {
            "data": {
                "top": [
                    {
                        "type": "Library Files",
                        "categories": [
                            {"name": "test/standalone/", "pages": [standalone]}
                        ],
                    },
                    {
                        "type": "Verification Files",
                        "categories": [
                            {"name": "test/standalone/", "pages": []},
                            {"name": "test/onlinejudge/", "pages": [onlinejudge]},
                        ],
                    },
                ]
            }
        }

        self.assertTrue(index_order.normalize_index(front_matter))
        index_order.validate_index(front_matter)

        top = front_matter["data"]["top"]
        self.assertEqual(top[0]["categories"], [])
        self.assertEqual(
            [category["name"] for category in top[1]["categories"]],
            ["test/onlinejudge/", "test/standalone/"],
        )
        self.assertEqual(top[1]["categories"][1]["pages"], [standalone])

        self.assertFalse(index_order.normalize_index(front_matter))
        index_order.validate_index(front_matter)

    def test_creates_destination_category_and_sorts_pages(self) -> None:
        z_page = page("test/onlinejudge/z.test.cpp", "warning")
        a_page = page("test/onlinejudge/a.test.cpp", "warning")
        front_matter = {
            "data": {
                "top": [
                    {
                        "type": "Library Files",
                        "categories": [
                            {
                                "name": "test/onlinejudge/",
                                "pages": [z_page, a_page],
                            }
                        ],
                    },
                    {"type": "Verification Files", "categories": []},
                ]
            }
        }

        self.assertTrue(index_order.normalize_index(front_matter))
        index_order.validate_index(front_matter)
        top = front_matter["data"]["top"]
        self.assertEqual(top[0]["categories"], [])
        self.assertEqual(
            [entry["path"] for entry in top[1]["categories"][0]["pages"]],
            [
                "test/onlinejudge/a.test.cpp",
                "test/onlinejudge/z.test.cpp",
            ],
        )

    def test_builds_linked_folder_hierarchy_before_library_files(self) -> None:
        front_matter = {
            "data": {
                "top": [
                    {
                        "type": "Library Files",
                        "categories": [
                            {
                                "name": "src/algorithm/graph/connectivity/",
                                "pages": [],
                            },
                            {
                                "name": "src/algorithm/graph/shortest_path/",
                                "pages": [],
                            },
                            {
                                "name": "src/structure/segtree/",
                                "pages": [],
                            },
                        ],
                    },
                    {"type": "Verification Files", "categories": []},
                ]
            }
        }

        self.assertTrue(index_order.normalize_index(front_matter))
        index_order.validate_index(front_matter)
        library = front_matter["data"]["top"][0]
        categories = library["categories"]
        anchors = {
            category["name"]: category["anchor"] for category in categories
        }
        content = "# Library\n\nIntroduction.\n\n<br>\n"
        normalized = index_order.normalize_content(front_matter, content)

        self.assertLess(normalized.index("<h2 id=\"contents\">Contents</h2>"),
                        normalized.index("<br>"))
        self.assertIn('<nav aria-labelledby="contents">', normalized)
        self.assertIn(">src/</a>", normalized)
        self.assertIn(">algorithm/</a>", normalized)
        self.assertIn(">graph/</a>", normalized)
        self.assertIn(">connectivity/</a>", normalized)
        self.assertIn(">shortest_path/</a>", normalized)
        self.assertIn(">structure/</a>", normalized)
        for anchor in anchors.values():
            self.assertIn(f'href="#{anchor}"', normalized)
        self.assertEqual(
            index_order.normalize_content(front_matter, normalized),
            normalized,
        )

        template = "before\n<h3>{{ category.name }}</h3>\nafter\n"
        normalized_template, changed = index_order.normalize_template(template)
        self.assertTrue(changed)
        self.assertIn(index_order.ANCHORED_CATEGORY_HEADING, normalized_template)
        second_template, changed = index_order.normalize_template(normalized_template)
        self.assertFalse(changed)
        self.assertEqual(second_template, normalized_template)

    def test_splits_library_files_into_three_linked_pages(self) -> None:
        front_matter = {
            "layout": "toppage",
            "data": {
                "top": [
                    {
                        "type": "Library Files",
                        "categories": [
                            {"name": "src/structure/segtree/", "pages": []},
                            {"name": "src/approximate/search/", "pages": []},
                            {"name": "src/integer_geometry/point/", "pages": []},
                            {"name": "src/algorithm/graph/", "pages": []},
                        ],
                    },
                    {"type": "Verification Files", "categories": []},
                ]
            },
        }
        index_order.normalize_index(front_matter)
        index_order.validate_index(front_matter)
        documents = index_order.split_index_documents(
            front_matter,
            "# Library\n\nIntroduction.\n\n<br>\n",
        )

        root_front_matter, root_content = index_order.split_front_matter(
            documents["index"]
        )
        self.assertEqual(
            [entry["type"] for entry in root_front_matter["data"]["top"]],
            ["Verification Files"],
        )
        for section in index_order.PAGE_SECTIONS:
            self.assertIn(f"'/{section}.html' | relative_url", root_content)

        section_front_matters = {}
        expected_categories = {
            "algorithm": ["src/algorithm/graph/", "src/integer_geometry/point/"],
            "structure": ["src/structure/segtree/"],
            "approximate": ["src/approximate/search/"],
        }
        for section in index_order.PAGE_SECTIONS:
            section_front_matter, section_body = index_order.split_front_matter(
                documents[section]
            )
            section_front_matters[section] = section_front_matter
            library = section_front_matter["data"]["top"][0]
            self.assertEqual(
                [category["name"] for category in library["categories"]],
                expected_categories[section],
            )
            self.assertIn('<h2 id="contents">Contents</h2>', section_body)
            self.assertIn("{{ '/' | relative_url }}", section_body)
        index_order.validate_split_documents(
            root_front_matter,
            section_front_matters,
        )

    def test_rejects_unsupported_category_heading_template(self) -> None:
        with self.assertRaisesRegex(ValueError, "unsupported"):
            index_order.normalize_template("<h3>different template</h3>")

    def test_validation_handles_unrelated_top_entries(self) -> None:
        valid_front_matter = {
            "data": {
                "top": [
                    {"type": "Library Files", "categories": []},
                    {"type": "Verification Files", "categories": []},
                    {"type": "Metadata"},
                ]
            }
        }
        index_order.validate_index(valid_front_matter)

        invalid_front_matter = {
            "data": {
                "top": [
                    {"type": "Library Files", "categories": []},
                    {"type": "Verification Files", "categories": []},
                    {
                        "type": "Other Files",
                        "categories": [
                            {
                                "name": "test/onlinejudge/",
                                "pages": [
                                    page(
                                        "test/onlinejudge/misplaced.test.cpp",
                                        "warning",
                                    )
                                ],
                            }
                        ],
                    },
                ]
            }
        }
        with self.assertRaisesRegex(ValueError, "outside Verification Files"):
            index_order.validate_index(invalid_front_matter)


if __name__ == "__main__":
    unittest.main()
