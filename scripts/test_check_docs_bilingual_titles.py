#!/usr/bin/env python3

from __future__ import annotations

import unittest

from check_docs_bilingual_titles import split_bilingual_title, validate_title


class CheckDocsBilingualTitlesTest(unittest.TestCase):
    def test_plain_bilingual_title(self) -> None:
        self.assertEqual(split_bilingual_title("Fenwick Tree (フェンウィック木)"), (
            "Fenwick Tree",
            "フェンウィック木",
        ))

    def test_problem_tags_follow_japanese_name(self) -> None:
        title = "Link Cut Tree (リンクカット木) [OTOCI] [jump_on_tree]"
        self.assertEqual(validate_title(title), [])
        self.assertEqual(split_bilingual_title(title), (
            "Link Cut Tree",
            "リンクカット木",
        ))

    def test_tag_does_not_replace_japanese_name(self) -> None:
        self.assertNotEqual(validate_title("Link Cut Tree [OTOCI]"), [])


if __name__ == "__main__":
    unittest.main()
