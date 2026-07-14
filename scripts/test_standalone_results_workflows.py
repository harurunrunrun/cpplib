#!/usr/bin/env python3

from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def parse_scalar(value: str) -> str | list[str]:
    value = value.strip()
    if value.startswith("[") and value.endswith("]"):
        inside = value[1:-1].strip()
        if not inside:
            return []
        return [str(parse_scalar(part)) for part in inside.split(",")]
    if len(value) >= 2 and value[0] == value[-1] and value[0] in "\"'":
        return value[1:-1]
    return value


def indentation(line: str) -> int:
    return len(line) - len(line.lstrip(" "))


def skip_blank(lines: list[str], index: int) -> int:
    while index < len(lines) and not lines[index].strip():
        index += 1
    return index


def parse_literal(
    lines: list[str], index: int, parent_indent: int
) -> tuple[str, int]:
    end = index
    while end < len(lines):
        line = lines[end]
        if line.strip() and indentation(line) <= parent_indent:
            break
        end += 1

    body = lines[index:end]
    content = [line for line in body if line.strip()]
    if not content:
        return "", end
    common_indent = min(indentation(line) for line in content)
    return "\n".join(
        line[common_indent:] if line.strip() else "" for line in body
    ), end


def parse_node(
    lines: list[str], index: int, indent: int
) -> tuple[dict | list, int]:
    index = skip_blank(lines, index)
    if lines[index].lstrip().startswith("- "):
        return parse_sequence(lines, index, indent)
    return parse_mapping(lines, index, indent)


def parse_mapping(
    lines: list[str], index: int, indent: int
) -> tuple[dict, int]:
    result: dict = {}
    while True:
        index = skip_blank(lines, index)
        if index >= len(lines) or indentation(lines[index]) < indent:
            break
        if indentation(lines[index]) != indent or lines[index].lstrip().startswith("- "):
            break

        content = lines[index].strip()
        if ":" not in content:
            raise ValueError(f"invalid workflow mapping line: {lines[index]!r}")
        key, raw_value = content.split(":", 1)
        raw_value = raw_value.strip()
        index += 1
        if raw_value in ("|", ">"):
            result[key], index = parse_literal(lines, index, indent)
        elif raw_value:
            result[key] = parse_scalar(raw_value)
        else:
            child = skip_blank(lines, index)
            if child >= len(lines) or indentation(lines[child]) <= indent:
                result[key] = {}
                index = child
            else:
                result[key], index = parse_node(
                    lines, child, indentation(lines[child])
                )
    return result, index


def parse_sequence(
    lines: list[str], index: int, indent: int
) -> tuple[list, int]:
    result: list = []
    while True:
        index = skip_blank(lines, index)
        if (
            index >= len(lines)
            or indentation(lines[index]) != indent
            or not lines[index].lstrip().startswith("- ")
        ):
            break

        content = lines[index].strip()[2:].strip()
        index += 1
        if ":" not in content:
            result.append(parse_scalar(content))
            continue

        key, raw_value = content.split(":", 1)
        item: dict = {key: parse_scalar(raw_value)} if raw_value.strip() else {}
        continuation = skip_blank(lines, index)
        if continuation < len(lines) and indentation(lines[continuation]) > indent:
            if raw_value.strip():
                extra, index = parse_mapping(lines, continuation, indent + 2)
                item.update(extra)
            else:
                item[key], index = parse_node(
                    lines, continuation, indentation(lines[continuation])
                )
                continuation = skip_blank(lines, index)
                if (
                    continuation < len(lines)
                    and indentation(lines[continuation]) == indent + 2
                ):
                    extra, index = parse_mapping(lines, continuation, indent + 2)
                    item.update(extra)
        result.append(item)
    return result, index


def load_workflow(name: str) -> dict:
    lines = (
        (ROOT / ".github" / "workflows" / name)
        .read_text(encoding="utf-8")
        .splitlines()
    )
    workflow, end = parse_node(lines, 0, 0)
    if skip_blank(lines, end) != len(lines) or not isinstance(workflow, dict):
        raise ValueError(f"could not parse complete workflow: {name}")
    return workflow


def named_steps(job: dict) -> dict[str, dict]:
    return {
        step["name"]: step
        for step in job["steps"]
        if "name" in step
    }


class StandaloneResultsWorkflowTest(unittest.TestCase):
    def test_standalone_preflight_runs_once_before_all_shards(self) -> None:
        workflow = load_workflow("verify.yaml")
        preflight = workflow["jobs"]["standalone-preflight"]
        shards = workflow["jobs"]["standalone-assets"]
        self.assertEqual(shards["needs"], "standalone-preflight")
        self.assertEqual(shards["if"], "${{ always() }}")
        self.assertEqual(
            named_steps(preflight)["Run standalone preflight"]["run"],
            "make standalone-assets-test",
        )
        shard_run = named_steps(shards)["Verify standalone generated assets"]["run"]
        self.assertIn("python3 scripts/run_standalone_assets.py", shard_run)
        self.assertNotIn("make standalone-assets", shard_run)
        source = (ROOT / ".github" / "workflows" / "verify.yaml").read_text(
            encoding="utf-8"
        )
        self.assertEqual(source.count("make standalone-assets-test"), 1)

    def test_verify_retries_only_with_successful_previous_results(self) -> None:
        workflow = load_workflow("verify.yaml")
        job = workflow["jobs"]["verify"]
        steps = named_steps(job)
        restore = steps["Restore previous verification result"]
        previous = steps["Filter restored successful verification results"]
        initial = steps["Verify shard ${{ matrix.index }} (attempt 1)"]
        first = steps["Prepare successful results for attempt 2"]
        second = steps["Prepare successful results for attempt 3"]
        retry_two = steps["Retry failed verification entries (attempt 2)"]
        retry_three = steps["Retry failed verification entries (attempt 3)"]
        self.assertGreaterEqual(int(job["timeout-minutes"]), 30)
        for attempt in (initial, retry_two, retry_three):
            self.assertEqual(attempt["continue-on-error"], "true")
            self.assertLess(
                int(attempt["with"]["timeout"]),
                60 * int(attempt["timeout-minutes"]),
            )
        self.assertEqual(
            restore["with"]["path"], ".competitive-verifier/cache/result.json"
        )
        for filter_step in (previous, first, second):
            self.assertIn("--success-only", filter_step["run"])
        self.assertEqual(
            retry_two["with"]["prev-result"],
            "${{ steps.retry_previous_1.outputs.path }}",
        )
        self.assertEqual(
            retry_three["with"]["prev-result"],
            "${{ steps.retry_previous_2.outputs.path }}",
        )
        self.assertIn("outcome != 'skipped'", second["if"])
        self.assertIn("outcome != 'skipped'", retry_three["if"])

    def test_merge_reports_details_and_caches_only_successes(self) -> None:
        workflow = load_workflow("verify.yaml")
        steps = named_steps(workflow["jobs"]["merge-and-check"])
        cache = steps["Prepare successful verification cache"]
        report = steps["Report verification failures"]
        save = steps["Save successful verification cache"]
        self.assertIn("--success-only", cache["run"])
        self.assertEqual(
            save["with"]["path"], ".competitive-verifier/cache/result.json"
        )
        self.assertIn(
            "scripts/report_competitive_verifier_failures.py", report["run"]
        )
        self.assertIn(".competitive-verifier/merged/result.json", report["run"])
        for message in (
            "The successful-result cache was not prepared",
            "The successful-result cache was not saved",
        ):
            self.assertIn(f'report_warning "{message}', report["run"])
            self.assertNotIn(f'report_error "{message}', report["run"])

    def test_verify_defers_shared_result_success_to_merged_check(self) -> None:
        workflow = load_workflow("verify.yaml")
        record = named_steps(workflow["jobs"]["verify"])["Record shard status"]

        self.assertNotIn("VERIFY_SUCCESS", record["env"])
        self.assertNotIn("reported_success", record["run"])
        self.assertIn(
            '"is_success": outcome == "success" and result_valid',
            record["run"],
        )

    def test_verify_uploads_every_standalone_shard_even_on_failure(self) -> None:
        workflow = load_workflow("verify.yaml")
        job = workflow["jobs"]["standalone-assets"]
        self.assertEqual(
            job["strategy"]["matrix"]["index"],
            ["0", "1", "2", "3"],
        )
        upload = named_steps(job)["Upload standalone verification results"]
        self.assertEqual(upload["if"], "${{ always() }}")
        self.assertEqual(upload["uses"], "actions/upload-artifact@v4")
        self.assertEqual(
            upload["with"]["name"],
            "standalone-results-${{ github.run_attempt }}-${{ matrix.index }}",
        )
        self.assertEqual(
            upload["with"]["path"],
            ".competitive-verifier/cache/standalone-results",
        )
        self.assertEqual(upload["with"]["if-no-files-found"], "error")

    def test_docs_workflow_run_uses_exact_trigger_context(self) -> None:
        workflow = load_workflow("docs.yaml")
        trigger = workflow["on"]["workflow_run"]
        self.assertEqual(trigger["workflows"], ["verify"])
        self.assertEqual(trigger["types"], ["completed"])
        self.assertEqual(trigger["branches"], ["main"])
        self.assertEqual(workflow["permissions"]["actions"], "read")
        self.assertEqual(
            workflow["concurrency"]["group"],
            "docs-${{ github.event_name == 'workflow_run' && github.event.workflow_run.head_branch || github.ref_name }}",
        )

        route = workflow["jobs"]["route"]
        self.assertEqual(route["permissions"]["contents"], "read")
        for output in (
            "should_build",
            "head_sha",
            "verify_run_id",
            "verify_run_attempt",
            "use_trigger_results",
            "deploy",
        ):
            self.assertIn(output, route["outputs"])
        script = named_steps(route)["Select documentation source and results"][
            "with"
        ]["script"]
        self.assertIn("context.payload.workflow_run", script)
        self.assertIn("headSha = run.head_sha", script)
        self.assertIn("verifyRunId = String(run.id)", script)
        self.assertIn("verifyRunAttempt = String(run.run_attempt)", script)
        self.assertIn("github.rest.repos.getBranch", script)
        self.assertIn("branch.data.commit.sha !== headSha", script)
        self.assertIn("files.some((file) => triggersVerify(file.filename))", script)
        self.assertIn("event === 'pull_request'", script)
        self.assertIn("event === 'workflow_dispatch'", script)

        docs = workflow["jobs"]["docs"]
        self.assertEqual(docs["needs"], "route")
        self.assertEqual(
            docs["if"],
            "${{ needs.route.outputs.should_build == 'true' }}",
        )
        self.assertEqual(docs["permissions"]["actions"], "read")
        checkout = named_steps(docs)["Checkout"]
        self.assertEqual(
            checkout["with"]["ref"],
            "${{ needs.route.outputs.head_sha }}",
        )

    def test_trigger_artifacts_override_cache_and_feed_docs(self) -> None:
        workflow = load_workflow("docs.yaml")
        docs = workflow["jobs"]["docs"]
        steps = docs["steps"]
        by_name = named_steps(docs)

        restore_online = by_name["Restore latest online verification result"]
        restore_standalone = by_name["Restore standalone verification results"]
        download_online = by_name[
            "Download online verification result from triggering run"
        ]
        download_standalone = by_name[
            "Download standalone verification results from triggering run"
        ]
        save_standalone = by_name["Save standalone verification result cache"]
        prepare = by_name["Prepare documentation result"]

        positions = {
            step["name"]: index
            for index, step in enumerate(steps)
            if "name" in step
        }
        self.assertLess(
            positions[restore_online["name"]],
            positions[download_online["name"]],
        )
        self.assertLess(
            positions[restore_standalone["name"]],
            positions[download_standalone["name"]],
        )
        self.assertLess(
            positions[download_standalone["name"]],
            positions[save_standalone["name"]],
        )
        self.assertLess(
            positions[save_standalone["name"]],
            positions[prepare["name"]],
        )

        self.assertEqual(download_online["uses"], "actions/download-artifact@v4")
        self.assertEqual(
            download_online["with"]["run-id"],
            "${{ needs.route.outputs.verify_run_id }}",
        )
        self.assertEqual(
            download_online["with"]["github-token"],
            "${{ secrets.GITHUB_TOKEN }}",
        )

        self.assertEqual(
            download_standalone["with"]["pattern"],
            "standalone-results-${{ needs.route.outputs.verify_run_attempt }}-*",
        )
        self.assertEqual(
            download_standalone["with"]["path"],
            ".competitive-verifier/cache/standalone-results",
        )
        self.assertEqual(download_standalone["with"]["merge-multiple"], "true")
        self.assertEqual(
            download_standalone["with"]["run-id"],
            "${{ needs.route.outputs.verify_run_id }}",
        )
        self.assertEqual(
            download_standalone["with"]["github-token"],
            "${{ secrets.GITHUB_TOKEN }}",
        )
        self.assertEqual(
            restore_standalone["with"]["key"],
            save_standalone["with"]["key"],
        )
        self.assertIn(
            "--standalone-results .competitive-verifier/cache/standalone-results",
            prepare["run"],
        )

    def test_pages_deployment_uses_route_decision(self) -> None:
        workflow = load_workflow("docs.yaml")
        docs = workflow["jobs"]["docs"]
        deploy_condition = "${{ needs.route.outputs.deploy == 'true' }}"
        page_steps = (
            "Setup Pages",
            "Build with Jekyll",
            "Remove unsafe polyfill.io reference",
            "Check generated site for unsafe CDN references",
            "Upload HTML artifact",
            "Upload Pages artifact",
        )
        steps = named_steps(docs)
        for name in page_steps:
            self.assertEqual(steps[name]["if"], deploy_condition)

        deploy = workflow["jobs"]["deploy"]
        self.assertEqual(deploy["needs"], ["route", "docs"])
        self.assertEqual(
            deploy["if"],
            "${{ needs.route.outputs.deploy == 'true' && needs.docs.result == 'success' }}",
        )


if __name__ == "__main__":
    unittest.main()
