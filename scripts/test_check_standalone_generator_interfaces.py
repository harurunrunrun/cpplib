#!/usr/bin/env python3

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

import check_standalone_generator_interfaces as interface_check


VALID_GENERATOR = """\\
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("--out-dir", required=True)
parser.parse_args()
"""

POSITIONAL_GENERATOR = """\\
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("out_dir")
parser.parse_args()
"""


def add_generator(root: Path, name: str, source: str) -> Path:
    (root / "test" / "standalone").mkdir(parents=True, exist_ok=True)
    directory = root / "test" / "generator" / name
    directory.mkdir(parents=True, exist_ok=True)
    (root / "test" / "standalone" / f"{name}.test.cpp").write_text(
        "// test\\n", encoding="utf-8"
    )
    generator = directory / "generator.py"
    generator.write_text(source, encoding="utf-8")
    return generator


class CheckStandaloneGeneratorInterfacesTest(unittest.TestCase):
    def test_discovery_only_includes_generators_for_standalone_tests(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            expected = add_generator(root, "covered", VALID_GENERATOR)
            orphan = root / "test" / "generator" / "orphan"
            orphan.mkdir(parents=True)
            (orphan / "generator.py").write_text(
                VALID_GENERATOR, encoding="utf-8"
            )
            self.assertEqual(interface_check.discover_generators(root), [expected])

    def test_valid_out_dir_interface_passes(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            generator = add_generator(root, "valid", VALID_GENERATOR)
            self.assertIsNone(
                interface_check.check_generator(
                    generator,
                    root=root,
                    timeout=2,
                )
            )

    def test_positional_output_directory_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            generator = add_generator(root, "positional", POSITIONAL_GENERATOR)
            failure = interface_check.check_generator(
                generator,
                root=root,
                timeout=2,
            )
            self.assertIsNotNone(failure)
            self.assertIn("does not advertise --out-dir", failure.message)

    def test_cpp_generator_contract_is_checked_without_style_assumptions(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "test" / "standalone").mkdir(parents=True)
            generator_directory = root / "test" / "generator" / "native"
            generator_directory.mkdir(parents=True)
            (root / "test" / "standalone" / "native.test.cpp").write_text(
                "// test\n", encoding="utf-8"
            )
            generator = generator_directory / "generator.cpp"
            generator.write_text(
                'int main() { /* accepts options via helper: "--out-dir" */ }\n',
                encoding="utf-8",
            )
            self.assertEqual(
                interface_check.discover_cpp_generators(root), [generator]
            )
            self.assertIsNone(interface_check.check_cpp_generator(generator))
            generator.write_text("int main() {}\n", encoding="utf-8")
            failure = interface_check.check_cpp_generator(generator)
            self.assertIsNotNone(failure)
            self.assertIn("does not advertise --out-dir", failure.message)


if __name__ == "__main__":
    unittest.main()
