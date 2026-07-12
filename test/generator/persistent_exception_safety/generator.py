#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    commands = [
        "TRIE_NODE",
        "TRIE_VERSION",
        "TRIE_ERASE_NODE",
        "TRIE_INVALID",
        "BINARY_NODE",
        "BINARY_VERSION",
        "BINARY_ERASE_NODE",
        "DSU_VERSION",
        "ARRAY_COPY",
        "CHT_NODE",
        "WAVELET_DEFAULT",
        "WEIGHTED_WAVELET_DEFAULT",
        "WAVELET_2D_DEFAULT",
        "WEIGHTED_WAVELET_2D_DEFAULT",
        "PARTIAL_WAVELET_DEFAULT",
        "PARTIAL_WEIGHTED_WAVELET_DEFAULT",
        "PARTIAL_FUNCTIONAL_WAVELET_DEFAULT",
        "PARTIAL_WAVELET_2D_DEFAULT",
        "PARTIAL_WEIGHTED_WAVELET_2D_DEFAULT",
        "LICHAO_VERSION",
    ]
    outputs = [
        "1 2 4 0 1",
        "1 1 1 0",
        "1 2 1 1",
        "1 1 1 1",
        "1 1 1 0",
        "1 1 1 0",
        "1 2 1 1",
        "1 2 1 0 1",
        "1 2 7 9",
        "1 1 1 1",
        "1 0 1 0",
        "1 0 1 0",
        "1 0 1 0",
        "1 0 1 0",
        "0 1 0",
        "0 1 0",
        "0 1 0",
        "0 1 0",
        "0 1 0",
        "1 2 1 7",
    ]
    (out_dir / "case_00.in").write_text(
        str(len(commands)) + "\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / "case_00.out").write_text(
        "\n".join(outputs) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
