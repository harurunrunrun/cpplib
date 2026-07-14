#!/usr/bin/env python3
"""List failed and skipped competitive-verifier entries with annotations."""

from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from pathlib import Path

from competitive_verifier.models import VerifyCommandResult


@dataclass(frozen=True)
class VerificationFailure:
    path: Path
    status: str
    message: str


def failure_details(result: VerifyCommandResult) -> list[VerificationFailure]:
    details = []
    for path, file_result in sorted(
        result.files.items(), key=lambda item: item[0].as_posix()
    ):
        if not file_result.verifications:
            details.append(
                VerificationFailure(path, "skipped", "no verification result")
            )
            continue
        for verification in file_result.verifications:
            status = verification.status.value
            if status == "success":
                continue
            name = verification.verification_name or "verification"
            message = f"{name}: {status}"
            failed_cases = []
            for testcase in verification.testcases or []:
                testcase_status = testcase.status.value
                if testcase_status != "AC":
                    failed_cases.append(f"{testcase.name} ({testcase_status})")
            if failed_cases:
                displayed = failed_cases[:8]
                message += "; failing testcases: " + ", ".join(displayed)
                if len(failed_cases) > len(displayed):
                    message += f", and {len(failed_cases) - len(displayed)} more"
            details.append(VerificationFailure(path, status, message))
    return details


def escape_workflow_value(value: str) -> str:
    return (
        value.replace("%", "%25")
        .replace("\r", "%0D")
        .replace("\n", "%0A")
        .replace(":", "%3A")
        .replace(",", "%2C")
    )


def report(details: list[VerificationFailure]) -> None:
    for detail in details:
        path = detail.path.as_posix()
        level = "warning" if detail.status == "skipped" else "error"
        print(f"[competitive-verifier] {path}: {detail.message}", file=sys.stderr)
        print(
            f"::{level} file={escape_workflow_value(path)},"
            f"title=competitive-verifier {detail.status}::"
            f"{escape_workflow_value(detail.message)}",
            file=sys.stderr,
        )


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--result", type=Path, required=True)
    args = parser.parse_args(argv)
    try:
        result = VerifyCommandResult.parse_file_relative(args.result)
    except (OSError, UnicodeError, ValueError) as error:
        print(
            f"could not read competitive-verifier result {args.result}: {error}",
            file=sys.stderr,
        )
        return 1
    details = failure_details(result)
    report(details)
    print(
        f"competitive-verifier diagnostics: {len(details)} failed/skipped "
        "verification(s)",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
