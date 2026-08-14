from pathlib import Path


_VERTICES = (
    "0 0 0\n2 0 0\n2 1 0\n1 1 0\n1 2 0\n0 2 0\n"
    "0 0 1\n2 0 1\n2 1 1\n1 1 1\n1 2 1\n0 2 1\n"
)
_FACES = (
    "2 1 0\n3 2 0\n3 0 5\n4 3 5\n"
    "6 7 8\n6 8 9\n11 6 9\n11 9 10\n"
    "0 1 7\n0 7 6\n1 2 8\n1 8 7\n2 3 9\n2 9 8\n"
    "3 4 10\n3 10 9\n4 5 11\n4 11 10\n5 0 6\n5 6 11\n"
)


def write_polyhedron3_case(out_dir: str, expected: str, query: str = "") -> None:
    out = Path(out_dir)
    out.mkdir(parents=True, exist_ok=True)
    data = "12 20\n" + _VERTICES + _FACES + query
    (out / "case_00.in").write_text(data, encoding="utf-8")
    (out / "case_00.out").write_text(expected, encoding="utf-8")
