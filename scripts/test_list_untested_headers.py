#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from list_untested_headers import untested_headers


class ListUntestedHeadersTest(unittest.TestCase):
    def test_only_headers_directly_targeted_by_a_test_are_covered(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            first = root / "src" / "first.hpp"
            second = root / "src" / "detail" / "second.hpp"
            missing = root / "src" / "missing.hpp"
            test = root / "test" / "standalone" / "sample.test.cpp"
            for path in (first, second, missing, test):
                path.parent.mkdir(parents=True, exist_ok=True)
            first.write_text('#include "detail/second.hpp"\n', encoding="utf-8")
            second.write_text("#pragma once\n", encoding="utf-8")
            missing.write_text("#pragma once\n", encoding="utf-8")
            test.write_text('#include "../../src/first.hpp"\n', encoding="utf-8")

            self.assertEqual(
                [path.name for path in untested_headers(root)],
                ["second.hpp", "missing.hpp"],
            )

    def test_thin_test_wrapper_does_not_count_as_direct_coverage(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            header = root / "src" / "target.hpp"
            implementation = root / "test" / "support" / "target_impl.cpp"
            standalone = root / "test" / "standalone" / "target.test.cpp"
            for path in (header, implementation, standalone):
                path.parent.mkdir(parents=True, exist_ok=True)
            header.write_text("#pragma once\n", encoding="utf-8")
            implementation.write_text(
                '#include "../../src/target.hpp"\n', encoding="utf-8"
            )
            standalone.write_text(
                '#include "../support/target_impl.cpp"\n',
                encoding="utf-8",
            )

            self.assertEqual(untested_headers(root), [header.resolve()])

    def test_test_helper_header_does_not_count_as_direct_coverage(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            header = root / "src" / "target.hpp"
            helper = root / "test" / "standalone" / "helper.hpp"
            test = root / "test" / "standalone" / "target.test.cpp"
            for path in (header, helper, test):
                path.parent.mkdir(parents=True, exist_ok=True)
            header.write_text("#pragma once\n", encoding="utf-8")
            helper.write_text(
                '#include "../../src/target.hpp"\n', encoding="utf-8"
            )
            test.write_text(
                '#include "helper.hpp"\n', encoding="utf-8"
            )

            self.assertEqual(untested_headers(root), [header.resolve()])

    def test_include_outside_root_is_ignored(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory) / "repo"
            header = root / "src" / "only.hpp"
            test = root / "test" / "onlinejudge" / "sample.test.cpp"
            header.parent.mkdir(parents=True)
            test.parent.mkdir(parents=True)
            header.write_text("#pragma once\n", encoding="utf-8")
            test.write_text('#include "../../../outside.hpp"\n', encoding="utf-8")

            self.assertEqual(untested_headers(root), [header.resolve()])


if __name__ == "__main__":
    unittest.main()
