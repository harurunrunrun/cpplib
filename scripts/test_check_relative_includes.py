#!/usr/bin/env python3

from __future__ import annotations

from pathlib import Path
from tempfile import TemporaryDirectory

import check_relative_includes


def main() -> None:
    with TemporaryDirectory() as directory:
        root = Path(directory)
        source = root / "src/library/example.hpp"
        sibling = root / "src/library/sibling.hpp"
        root_header = root / "src/shared.hpp"
        source.parent.mkdir(parents=True)
        sibling.write_text("#ifndef SIBLING\n#define SIBLING\n#endif\n")
        root_header.write_text("#ifndef SHARED\n#define SHARED\n#endif\n")
        source.write_text(
            '#include "sibling.hpp"\n'
            '#include "src/shared.hpp"\n'
            '#include <vector>\n',
            encoding="utf-8",
        )
        assert check_relative_includes.relative_include_violations(
            root, [Path("src")]
        ) == []

        source.write_text(
            '#include "missing.hpp"\n',
            encoding="utf-8",
        )
        errors = check_relative_includes.relative_include_violations(
            root, [Path("src")]
        )
        assert errors == [
            "src/library/example.hpp:1: "
            "quoted include does not resolve: missing.hpp"
        ]

        source.write_text(
            '#include "../../../outside.hpp"\n',
            encoding="utf-8",
        )
        outside = root.parent / "outside.hpp"
        outside.write_text("outside\n", encoding="utf-8")
        try:
            errors = check_relative_includes.relative_include_violations(
                root, [Path("src")]
            )
            assert errors and "does not resolve" in errors[0]
        finally:
            outside.unlink()

    print("relative include checker tests passed")


if __name__ == "__main__":
    main()
