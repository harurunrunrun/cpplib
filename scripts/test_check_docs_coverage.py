#!/usr/bin/env python3

from pathlib import Path
from tempfile import TemporaryDirectory

import check_docs_coverage


def main() -> None:
    with TemporaryDirectory() as directory:
        root = Path(directory)
        source = root / "src"
        docs = root / "docs"
        header = source / "algorithm" / "graph" / "sample.hpp"
        document = docs / "algorithm" / "graph" / "sample.md"
        other = docs / "structure" / "sample.md"
        header.parent.mkdir(parents=True)
        document.parent.mkdir(parents=True)
        other.parent.mkdir(parents=True)
        header.write_text("#ifndef SAMPLE\n#define SAMPLE\n#endif\n")
        document.write_text(
            "---\n"
            "title: Sample (標本)\n"
            "documentation_of: ../../../src/algorithm/graph/sample.hpp\n"
            "---\n"
            "\n## 時間計算量\n\nO(1)。\n\n## 注意点\n\nなし。\n"
        )
        other.write_text("not front matter\n")

        assert check_docs_coverage.documentation_files(docs) == [
            document,
            other,
        ]
        assert check_docs_coverage.expected_documentation_path(
            header, source, docs
        ) == document
        assert check_docs_coverage.expected_documentation_path(
            root / "outside.hpp", source, docs
        ) is None
        assert check_docs_coverage.documentation_target(document) == (
            header.resolve(),
            [],
        )

    print("docs coverage checker tests passed")


if __name__ == "__main__":
    main()
