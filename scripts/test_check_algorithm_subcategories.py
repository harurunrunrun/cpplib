#!/usr/bin/env python3

from pathlib import Path
from tempfile import TemporaryDirectory

import check_algorithm_subcategories


def main() -> None:
    with TemporaryDirectory() as directory:
        root = Path(directory)
        layout = {
            Path("src/algorithm/sample"): frozenset({"first", "second"}),
        }
        first = root / "src/algorithm/sample/first/value.hpp"
        second = root / "src/algorithm/sample/second/value.hpp"
        first.parent.mkdir(parents=True)
        second.parent.mkdir(parents=True)
        first.write_text("#ifndef FIRST\n#define FIRST\n#endif\n")
        second.write_text("#ifndef SECOND\n#define SECOND\n#endif\n")
        assert check_algorithm_subcategories.layout_violations(
            root, layout
        ) == []

        flat = root / "src/algorithm/sample/flat.hpp"
        flat.write_text("#ifndef FLAT\n#define FLAT\n#endif\n")
        errors = check_algorithm_subcategories.layout_violations(root, layout)
        assert any("declared subcategory" in error for error in errors)
        flat.unlink()

        unknown = root / "src/algorithm/sample/third/value.hpp"
        unknown.parent.mkdir()
        unknown.write_text("#ifndef THIRD\n#define THIRD\n#endif\n")
        errors = check_algorithm_subcategories.layout_violations(root, layout)
        assert any("unknown subcategory" in error for error in errors)
        unknown.unlink()
        unknown.parent.rmdir()

        nested = root / "src/algorithm/sample/first/detail/value.hpp"
        nested.parent.mkdir()
        nested.write_text("#ifndef NESTED\n#define NESTED\n#endif\n")
        errors = check_algorithm_subcategories.layout_violations(root, layout)
        assert any("must not be nested" in error for error in errors)
        nested.unlink()
        nested.parent.rmdir()

        second.unlink()
        errors = check_algorithm_subcategories.layout_violations(root, layout)
        assert any("category has no header" in error for error in errors)

        missing_layout = {
            Path("src/algorithm/missing"): frozenset({"category"}),
        }
        errors = check_algorithm_subcategories.layout_violations(
            root, missing_layout
        )
        assert any("directory is missing" in error for error in errors)

    print("algorithm subcategory checker tests passed")


if __name__ == "__main__":
    main()
