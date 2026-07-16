#!/usr/bin/env python3
"""Tests for check_docs_markdown.py."""

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from check_docs_markdown import check_file


class CheckDocsMarkdownTest(unittest.TestCase):
    def check(self, body: str) -> list[str]:
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "sample.md"
            path.write_text(body, encoding="utf-8")
            return check_file(path)

    def test_valid_markdown(self) -> None:
        tick = chr(96)
        body = (
            "| API | cost |\n"
            "| --- | --- |\n"
            "| f | $O(\\lvert x\\rvert)$ |\n\n"
            f"{tick}std::vector<T>{tick} and [source](https://example.com/a.md)\n"
            "$i<j<k$ and $a_i>a_j>a_k$.\n"
        )
        self.assertEqual(self.check(body), [])

    def test_table_extra_pipe_is_rejected(self) -> None:
        errors = self.check(
            "| API | cost |\n"
            "| --- | --- |\n"
            "| f | $O(|x|)$ |\n"
        )
        self.assertTrue(any("table has" in error for error in errors))

    def test_table_missing_cell_is_rejected(self) -> None:
        errors = self.check(
            "| API | time | space |\n"
            "| --- | --- | --- |\n"
            "| field | $O(1)$ |\n"
        )
        self.assertTrue(any("table has" in error for error in errors))

    def test_raw_template_tag_is_rejected(self) -> None:
        errors = self.check("std::vector<T> is used.\n")
        self.assertTrue(any("raw HTML-like" in error for error in errors))

    def test_code_hides_template_tag_from_html_parser(self) -> None:
        tick = chr(96)
        self.assertEqual(self.check(f"{tick}std::vector<T>{tick}\n"), [])
        self.assertEqual(
            self.check(f"{tick * 3}cpp\nstd::vector<T> value;\n{tick * 3}\n"),
            [],
        )

    def test_relative_markdown_link_is_rejected(self) -> None:
        errors = self.check("[Result](result.md)\n")
        self.assertTrue(any("relative Markdown link" in error for error in errors))

    def test_unclosed_code_is_rejected(self) -> None:
        tick = chr(96)
        errors = self.check(f"{tick}value\n")
        self.assertTrue(any("inline-code span" in error for error in errors))
        errors = self.check(f"{tick * 3}cpp\nint value;\n")
        self.assertTrue(any("fenced code block" in error for error in errors))


if __name__ == "__main__":
    unittest.main()
