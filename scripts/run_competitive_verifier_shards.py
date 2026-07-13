#!/usr/bin/env python3
"""Run competitive-verifier splits concurrently and merge every valid result."""

from __future__ import annotations

import argparse
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from competitive_verifier.models import VerifyCommandResult

from filter_competitive_verifier_result import (
    ResultFilterError,
    filter_result,
    read_plan_paths,
    read_result,
    write_result_atomic,
)


def merge_result_files(paths: list[Path]) -> VerifyCommandResult:
    if not paths:
        raise ValueError("no shard results to merge")
    result = read_result(paths[0])
    for path in paths[1:]:
        result = result.merge(read_result(path))
    return result


def run_shard(
    *,
    verifier: Path,
    verify_json: Path,
    output: Path,
    previous: Path | None,
    split_size: int,
    split_index: int,
) -> int:
    command = [
        str(verifier),
        "verify",
        "--verify-json",
        str(verify_json),
        "--output",
        str(output),
        "--split",
        str(split_size),
        "--split-index",
        str(split_index),
    ]
    if previous is not None:
        command.extend(["--prev-result", str(previous)])
    print(
        f"[competitive-verifier] START shard {split_index}/{split_size}",
        file=sys.stderr,
    )
    try:
        completed = subprocess.run(command, stdout=subprocess.DEVNULL, check=False)
    except OSError as error:
        print(
            f"[competitive-verifier] shard {split_index} could not start: {error}",
            file=sys.stderr,
        )
        return 127
    print(
        f"[competitive-verifier] END shard {split_index}/{split_size} "
        f"(exit {completed.returncode})",
        file=sys.stderr,
    )
    return completed.returncode


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--verifier", type=Path, required=True)
    parser.add_argument("--verify-json", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--prev-result", type=Path)
    parser.add_argument("--shard-dir", type=Path, required=True)
    parser.add_argument("--jobs", type=int, required=True)
    args = parser.parse_args(argv)

    if args.jobs <= 0:
        parser.error("--jobs must be positive")
    if not args.verifier.is_file():
        parser.error(f"verifier does not exist: {args.verifier}")
    if not args.verify_json.is_file():
        parser.error(f"verification plan does not exist: {args.verify_json}")
    try:
        planned_paths = read_plan_paths(args.verify_json)
    except (OSError, UnicodeError, ValueError, ResultFilterError) as error:
        parser.error(f"invalid verification plan: {error}")

    previous = args.prev_result
    if previous is not None and not previous.is_file():
        previous = None

    args.shard_dir.mkdir(parents=True, exist_ok=True)
    outputs = [args.shard_dir / f"result-{index}.json" for index in range(args.jobs)]
    if args.output.resolve() in {output.resolve() for output in outputs}:
        parser.error("--output must not be one of the per-shard output paths")
    # Remove only this invocation's exact outputs.  Deleting the entire caller-
    # supplied directory could destroy unrelated cache files, while retaining an
    # old result-N.json could make a crashed shard look valid.
    for output in outputs:
        output.unlink(missing_ok=True)

    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = [
            executor.submit(
                run_shard,
                verifier=args.verifier,
                verify_json=args.verify_json,
                output=outputs[index],
                previous=previous,
                split_size=args.jobs,
                split_index=index,
            )
            for index in range(args.jobs)
        ]
        return_codes = [future.result() for future in futures]

    valid_outputs: list[Path] = []
    invalid_indices: list[int] = []
    for index, path in enumerate(outputs):
        try:
            read_result(path)
        except Exception as error:  # competitive-verifier exposes pydantic errors here
            print(
                f"[competitive-verifier] invalid shard {index}: {error}",
                file=sys.stderr,
            )
            invalid_indices.append(index)
        else:
            valid_outputs.append(path)

    if valid_outputs:
        merged = filter_result(merge_result_files(valid_outputs), planned_paths)
        write_result_atomic(args.output, merged)
        print(
            f"[competitive-verifier] saved {len(merged.files)} file result(s) "
            f"from {len(valid_outputs)}/{args.jobs} shard(s) to {args.output}",
            file=sys.stderr,
        )
    else:
        print(
            "[competitive-verifier] no valid shard result; previous result was preserved",
            file=sys.stderr,
        )
        return 1

    failed_processes = [
        index for index, return_code in enumerate(return_codes) if return_code != 0
    ]
    merged_paths = set(merged.files)
    missing_paths = planned_paths - merged_paths
    if failed_processes or invalid_indices or missing_paths or not merged.is_success():
        if failed_processes:
            print(
                f"[competitive-verifier] failed shard processes: {failed_processes}",
                file=sys.stderr,
            )
        if invalid_indices:
            print(
                f"[competitive-verifier] invalid shard results: {invalid_indices}",
                file=sys.stderr,
            )
        if missing_paths:
            rendered = ", ".join(sorted(path.as_posix() for path in missing_paths))
            print(
                f"[competitive-verifier] missing planned file results: {rendered}",
                file=sys.stderr,
            )
        if not merged.is_success():
            print("[competitive-verifier] one or more files failed", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
