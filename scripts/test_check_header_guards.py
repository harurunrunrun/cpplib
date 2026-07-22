#!/usr/bin/env python3

from contextlib import redirect_stderr
from io import StringIO
from pathlib import Path
from tempfile import TemporaryDirectory

import check_header_guards


def expect_failure(
    inputs: list[Path],
    root: Path,
    fix: bool,
) -> int:
    with redirect_stderr(StringIO()):
        return check_header_guards.run(inputs, root, fix)


def main() -> None:
    with TemporaryDirectory() as directory:
        root = Path(directory)
        source = root / "src" / "space name"
        support = root / "test" / "support"
        source.mkdir(parents=True)
        support.mkdir(parents=True)

        pragma_header = source / "sample-file.hpp"
        pragma_header.write_bytes(
            b"  # pragma once  // supported spelling\r\n\r\n"
            b"#if 1\r\ninline int sample(){ return 1; }\r\n#endif\r\n"
        )
        guarded_header = support / "guarded.hpp"
        guarded_header.write_text(
            "#ifndef OLD_GUARD\n"
            "#define OLD_GUARD\n\n"
            "inline int guarded(){ return 2; }\n\n"
            "#endif\n",
            encoding="utf-8",
        )
        paths = check_header_guards.header_paths([
            root / "src", root / "test"
        ])
        assert check_header_guards.run(paths, root, True) == 0
        assert check_header_guards.violations(paths, root) == []

        expected = "CPPLIB_SRC_SPACE_NAME_SAMPLE_FILE_HPP_INCLUDED"
        converted = pragma_header.read_text(encoding="utf-8")
        assert converted.startswith(
            f"#ifndef {expected}\n#define {expected}\n"
        )
        assert converted.endswith(f"#endif  // {expected}\n")
        assert "pragma once" not in converted
        assert "#if 1\ninline int sample(){ return 1; }\n#endif" in converted
        assert b"\r\n" not in pragma_header.read_bytes()

        guarded_expected = "CPPLIB_TEST_SUPPORT_GUARDED_HPP_INCLUDED"
        converted_guarded = guarded_header.read_text(encoding="utf-8")
        assert converted_guarded.startswith(
            f"#ifndef {guarded_expected}\n"
            f"#define {guarded_expected}\n"
        )
        assert converted_guarded.endswith(
            f"#endif  // {guarded_expected}\n"
        )

        wrong_guard = converted_guarded.replace(
            f"#define {guarded_expected}",
            "#define WRONG_GUARD",
            1,
        )
        guarded_header.write_text(wrong_guard, encoding="utf-8")
        errors = check_header_guards.violations(paths, root)
        assert any("second line" in error for error in errors)
        guarded_header.write_text(converted_guarded, encoding="utf-8")

        before_second_fix = {
            path: path.read_bytes() for path in paths
        }
        assert check_header_guards.run(paths, root, True) == 0
        assert before_second_fix == {
            path: path.read_bytes() for path in paths
        }

        malformed = support / "malformed.hpp"
        malformed.write_text(
            "#ifndef OLD\n#define OLD\n#endif_BAD\n",
            encoding="utf-8",
        )
        untouched = source / "untouched.hpp"
        untouched.write_text(
            "#pragma once\ninline int untouched = 3;\n",
            encoding="utf-8",
        )
        original_untouched = untouched.read_bytes()
        assert expect_failure([untouched, malformed], root, True) == 2
        assert untouched.read_bytes() == original_untouched

        nested_pragma = support / "nested_pragma.hpp"
        nested_pragma.write_text(
            "#pragma once\n  # pragma once\n",
            encoding="utf-8",
        )
        original_nested = nested_pragma.read_bytes()
        assert expect_failure([untouched, nested_pragma], root, True) == 2
        assert untouched.read_bytes() == original_untouched
        assert nested_pragma.read_bytes() == original_nested

        bom_header = support / "bom.hpp"
        bom_header.write_bytes(b"\xef\xbb\xbf#pragma once\n")
        assert expect_failure([untouched, bom_header], root, True) == 2
        assert untouched.read_bytes() == original_untouched

        collision_dash = source / "same-name.hpp"
        collision_underscore = source / "same_name.hpp"
        for header in (collision_dash, collision_underscore):
            header.write_text("#pragma once\n", encoding="utf-8")
        collision_before = {
            header: header.read_bytes()
            for header in (collision_dash, collision_underscore)
        }
        assert expect_failure(
            [collision_dash, collision_underscore], root, True
        ) == 2
        assert collision_before == {
            header: header.read_bytes()
            for header in (collision_dash, collision_underscore)
        }

        assert expect_failure(
            [root / "missing", untouched], root, True
        ) == 2
        empty = root / "empty"
        empty.mkdir()
        assert expect_failure([empty], root, False) == 2

        outside_target = root.parent / f"{root.name}_outside_header.hpp"
        symlink = source / "outside.hpp"
        outside_target.write_text("#pragma once\n", encoding="utf-8")
        try:
            symlink.symlink_to(outside_target)
            assert expect_failure([symlink], root, True) == 2
        finally:
            outside_target.unlink(missing_ok=True)

    print("header guard checker tests passed")


if __name__ == "__main__":
    main()
