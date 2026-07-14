#!/usr/bin/env python3

from __future__ import annotations

import unittest
from pathlib import Path

import yaml


ROOT = Path(__file__).resolve().parent.parent


def load_workflow(name: str) -> dict:
    return yaml.load(
        (ROOT / ".github" / "workflows" / name).read_text(encoding="utf-8"),
        Loader=yaml.BaseLoader,
    )


def named_steps(job: dict) -> dict[str, dict]:
    return {
        step["name"]: step
        for step in job["steps"]
        if "name" in step
    }


class StandaloneResultsWorkflowTest(unittest.TestCase):
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
