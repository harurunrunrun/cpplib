#!/usr/bin/env python3
"""Tests for check_wavelet_matrix_design.py."""

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from check_wavelet_matrix_design import find_violations


class CheckWaveletMatrixDesignTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary_directory.cleanup)
        self.root = Path(self.temporary_directory.name) / "wavelet_matrix"
        self.root.mkdir()

    def write(self, relative_path: str, source: str) -> None:
        path = self.root / relative_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(source, encoding="utf-8")

    def details(self) -> list[str]:
        violations, _ = find_violations(self.root)
        return [violation.detail for violation in violations]

    def test_accepts_deterministic_btrees_and_independent_partial_storage(self) -> None:
        self.write(
            "detail/mutable_btree_bit_sequence.hpp",
            "class MutableBTreeBitSequence {};\n",
        )
        self.write(
            "dynamic/dynamic_wavelet_matrix.hpp",
            '#include "../detail/mutable_btree_bit_sequence.hpp"\n',
        )
        self.write(
            "partially_persistent/partially_persistent_wavelet_matrix.hpp",
            '#include "../detail/partially_persistent_bit_sequence.hpp"\n',
        )
        self.assertEqual(self.details(), [])

    def test_rejects_block_size_compatibility_parameter(self) -> None:
        self.write(
            "persistent/persistent_wavelet_matrix.hpp",
            "template<int BLOCK_SIZE> struct PersistentWaveletMatrix {};\n",
        )
        self.assertIn(
            "BLOCK_SIZE compatibility parameters are forbidden",
            self.details(),
        )

    def test_rejects_obsolete_square_decomposition_members(self) -> None:
        self.write(
            "detail/old_storage.hpp",
            "int block_index; int block_sorted;\n",
        )
        details = self.details()
        self.assertEqual(details.count("square-decomposition block state is forbidden"), 2)

    def test_rejects_persistent_block_reference(self) -> None:
        self.write(
            "detail/persistent_block_reference.hpp",
            "struct PersistentBlockReference {};\n",
        )
        self.assertEqual(
            self.details(),
            ["the persistent block-reference implementation is forbidden"],
        )

    def test_rejects_randomized_dynamic_sequence_trees(self) -> None:
        self.write(
            "dynamic/dynamic_wavelet_matrix.hpp",
            "std::mt19937_64 random; int treap_priority = rng();\n",
        )
        details = self.details()
        self.assertIn(
            "dynamic Wavelet Matrix storage must not depend on randomized priorities",
            details,
        )
        self.assertIn(
            "dynamic Wavelet Matrix storage must not be a randomized treap",
            details,
        )

    def test_rejects_partial_wrapper_dependency_on_full_persistence(self) -> None:
        self.write(
            "partially_persistent/example.hpp",
            '#include "../persistent/persistent_wavelet_matrix.hpp"\n',
        )
        self.assertEqual(
            self.details(),
            [
                "partially persistent structures must not include fully persistent wrappers"
            ],
        )

    def test_rejects_partial_use_of_full_persistence_core(self) -> None:
        self.write(
            "partially_persistent/example.hpp",
            "using Storage = PersistentBTreeBitSequence<int, int>;\n",
        )
        self.assertEqual(
            self.details(),
            [
                "partially persistent structures must not use fully persistent implementations"
            ],
        )


    def test_missing_root_fails_closed(self) -> None:
        missing = self.root / "missing"
        violations, checked = find_violations(missing)
        self.assertEqual(checked, 0)
        self.assertEqual(len(violations), 1)
        self.assertIn("does not exist", violations[0].detail)


if __name__ == "__main__":
    unittest.main()
