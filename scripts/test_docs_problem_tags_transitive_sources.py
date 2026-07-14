#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from docs_problem_tags import primary_sources


class DocsProblemTagsTransitiveSourcesTest(unittest.TestCase):
    def test_descriptor_selects_function_header_from_umbrella(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = (
                root
                / "test"
                / "onlinejudge"
                / "cgl_1_a.projection.test.cpp"
            )
            umbrella = root / "src" / "algorithm" / "geometry" / "geometry.hpp"
            projection = umbrella.parent / "projection.hpp"
            wrapper.parent.mkdir(parents=True)
            umbrella.parent.mkdir(parents=True)
            umbrella.write_text('#include "projection.hpp"\n', encoding="utf-8")
            projection.write_text("#pragma once\n", encoding="utf-8")
            source = '#include "../../src/algorithm/geometry/geometry.hpp"\n'

            self.assertEqual(primary_sources(wrapper, source), [projection.resolve()])


    def assert_umbrella_primary(
        self,
        wrapper_name: str,
        header_names: list[str],
        expected_name: str,
    ) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            wrapper = root / "test" / "onlinejudge" / wrapper_name
            umbrella = root / "src" / "algorithm" / "geometry" / "geometry.hpp"
            wrapper.parent.mkdir(parents=True)
            umbrella.parent.mkdir(parents=True)
            umbrella.write_text(
                "".join(f'#include "{name}.hpp"\n' for name in header_names),
                encoding="utf-8",
            )
            for name in header_names:
                (umbrella.parent / f"{name}.hpp").write_text(
                    "#pragma once\n", encoding="utf-8"
                )
            source = '#include "../../src/algorithm/geometry/geometry.hpp"\n'

            self.assertEqual(
                [path.stem for path in primary_sources(wrapper, source)],
                [expected_name],
            )

    def test_longest_partial_match_wins(self) -> None:
        self.assert_umbrella_primary(
            "cgl_2_c.segment_cross_point.test.cpp",
            ["cross", "cross_point", "circle_segment_cross_points"],
            "cross_point",
        )

    def test_more_specific_header_containing_descriptor_wins(self) -> None:
        self.assert_umbrella_primary(
            "cgl_7_e.circle_cross_points.test.cpp",
            ["cross_point", "circle_circle_cross_points"],
            "circle_circle_cross_points",
        )

    def test_point_in_polygon_alias_selects_contains(self) -> None:
        self.assert_umbrella_primary(
            "cgl_3_c.point_in_polygon.test.cpp", ["area", "contains"], "contains"
        )


if __name__ == "__main__":
    unittest.main()
