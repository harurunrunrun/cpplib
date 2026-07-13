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
            ["src/algorithm/", "src/structure/"],
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
