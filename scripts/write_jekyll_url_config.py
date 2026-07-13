#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Write a Jekyll override config from JEKYLL_URL."
    )
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()

    url = os.environ.get("JEKYLL_URL")
    if not url:
        parser.error("JEKYLL_URL must be non-empty")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(
        json.dumps({"url": url}, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
