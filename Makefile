SHELL := /bin/bash

PYTHON ?= python3
COMPETITIVE_VERIFIER_VERSION ?= 4.1.2

VERIFIER_ROOT := .competitive-verifier
VERIFIER_VENV := $(VERIFIER_ROOT)/venv
VERIFIER := $(VERIFIER_VENV)/bin/competitive-verifier
VERIFIER_CACHE := $(VERIFIER_ROOT)/cache
VERIFY_FILES := $(VERIFIER_CACHE)/verify-files.json
DOCS_VERIFY_FILES := $(VERIFIER_CACHE)/docs-verify-files.json
VERIFY_RESULT := $(VERIFIER_CACHE)/result.json
DOCS_RESULT := $(VERIFIER_CACHE)/docs-result.json
DOCS_SOURCE := $(VERIFIER_ROOT)/_jekyll
DOCS_OUTPUT := $(VERIFIER_ROOT)/site
BUNDLE_PATH := $(VERIFIER_ROOT)/vendor/bundle
STANDALONE_ASSET_CACHE := $(VERIFIER_CACHE)/standalone-assets
STANDALONE_RESULT_DIR := $(VERIFIER_CACHE)/standalone-results
GXX13 ?= g++-13
CXX := $(GXX13)
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra
STANDALONE_SPLIT_SIZE ?= 1
STANDALONE_SPLIT_INDEX ?= 0
LOCAL_VERIFY_JOBS ?= 4
VERIFIER_GXX_WRAPPER_DIR := $(CURDIR)/scripts/competitive_verifier_gcc13
VERIFIER_REAL_GXX := $(shell command -v $(GXX13) 2>/dev/null)
VERIFIER_COMMAND_ENV := COMPETITIVE_VERIFIER_REAL_GXX="$(VERIFIER_REAL_GXX)" PATH="$(VERIFIER_GXX_WRAPPER_DIR):$(PATH)"

COMMA := ,
JEKYLL_URL_CONFIG := $(VERIFIER_CACHE)/jekyll-url.yml
JEKYLL_URL_CONFIG_COMMAND := $(if $(strip $(JEKYLL_URL)),$(PYTHON) scripts/write_jekyll_url_config.py --output "$(JEKYLL_URL_CONFIG)")
JEKYLL_BUILD_ARGS := $(strip \
	$(if $(strip $(JEKYLL_URL)),--config "$(abspath $(DOCS_SOURCE)/_config.yml)$(COMMA)$(abspath $(JEKYLL_URL_CONFIG))") \
	$(if $(strip $(JEKYLL_BASEURL)),--baseurl "$(JEKYLL_BASEURL)") \
)

.PHONY: help gcc13-check verifier-setup verifier-wrapper-test header-guard-check algorithm-layout-check no-boost-dependency-check test-verifier-markers verifier-resolve docs-verifier-resolve test-coverage-check standalone-generator-interface-check standalone-assets-test standalone-results-prune standalone-results-check standalone-assets verify docs-title-check docs-coverage-check docs-source docs-prerequisites docs docs-serve verifier-clean

help:
	@echo "make verify  competitive-verifierでtestを実行"
	@echo "make verify LOCAL_VERIFY_JOBS=N  local verifyの並列数を指定 (default: $(LOCAL_VERIFY_JOBS))"
	@echo "make standalone-assets  standalone testのgenerator/checkerを実行"
	@echo "make standalone-results-prune  削除・改名済みstandaloneの古いmanifestを除去"
	@echo "make standalone-results-check  standalone全件の最新成功manifestを検査"
	@echo "make header-guard-check  全headerの一意なinclude guardを検査"
	@echo "make algorithm-layout-check  validate algorithm subcategories"
	@echo "make no-boost-dependency-check  reject Boost dependencies in src/test/docs"
	@echo "make docs-title-check  docsの英日併記タイトルを検査"
	@echo "make test-coverage-check  全headerに直接対象テストがあることを検査"
	@echo "make docs-coverage-check  全headerのdocsと注意点見出しを検査"
	@echo "make docs    testを実行せずHTMLを$(DOCS_OUTPUT)へ生成"
	@echo "make docs-serve  HTMLを生成してlocalhost:4000で配信"
	@echo "make verifier-clean  ローカル生成物とvenvを削除"

gcc13-check:
	@if ! compiler="$$(command -v "$(GXX13)" 2>/dev/null)"; then \
		echo "GCC 13 is required: $(GXX13) was not found" >&2; \
		exit 1; \
	fi; \
	version="$$("$$compiler" -dumpfullversion -dumpversion 2>/dev/null)" || { \
		echo "failed to query GCC version: $$compiler" >&2; \
		exit 1; \
	}; \
	case "$$version" in 13|13.*) ;; *) \
		echo "GCC 13 is required, but $$compiler is $$version" >&2; \
		exit 1; \
	esac

$(VERIFIER):
	$(PYTHON) -m venv $(VERIFIER_VENV)
	$(VERIFIER_VENV)/bin/pip install "competitive-verifier==$(COMPETITIVE_VERIFIER_VERSION)"

verifier-setup: $(VERIFIER)

verifier-wrapper-test:
	$(PYTHON) scripts/test_competitive_verifier_gcc13_wrapper.py

header-guard-check:
	$(PYTHON) scripts/test_check_header_guards.py
	$(PYTHON) scripts/check_header_guards.py src test

algorithm-layout-check:
	$(PYTHON) scripts/test_check_algorithm_subcategories.py
	$(PYTHON) scripts/check_algorithm_subcategories.py

no-boost-dependency-check: header-guard-check algorithm-layout-check
	$(PYTHON) scripts/test_check_no_boost_dependency.py
	$(PYTHON) scripts/check_no_boost_dependency.py src test docs

test-verifier-markers:
	$(PYTHON) scripts/test_check_test_verifier_markers.py
	$(PYTHON) scripts/check_test_verifier_markers.py

verifier-resolve: gcc13-check verifier-setup verifier-wrapper-test no-boost-dependency-check test-verifier-markers
	@mkdir -p $(VERIFIER_CACHE)
	$(VERIFIER_COMMAND_ENV) $(VERIFIER) oj-resolve --include src test/onlinejudge --config config.toml > $(VERIFY_FILES).tmp
	$(PYTHON) scripts/check_unsupported_onlinejudge_assets.py
	$(PYTHON) scripts/test_normalize_competitive_verifier_plan.py
	$(VERIFIER_VENV)/bin/python scripts/test_filter_competitive_verifier_result.py
	$(VERIFIER_VENV)/bin/python scripts/test_report_competitive_verifier_failures.py
	$(PYTHON) scripts/normalize_competitive_verifier_plan.py $(VERIFY_FILES).tmp
	mv $(VERIFY_FILES).tmp $(VERIFY_FILES)

docs-verifier-resolve: gcc13-check verifier-setup verifier-wrapper-test no-boost-dependency-check test-verifier-markers
	@mkdir -p $(VERIFIER_CACHE)
	$(VERIFIER_COMMAND_ENV) $(VERIFIER) oj-resolve --include src test/onlinejudge test/standalone --config config.toml > $(DOCS_VERIFY_FILES).tmp
	$(PYTHON) scripts/test_normalize_competitive_verifier_plan.py
	$(PYTHON) scripts/normalize_competitive_verifier_plan.py $(DOCS_VERIFY_FILES).tmp
	mv $(DOCS_VERIFY_FILES).tmp $(DOCS_VERIFY_FILES)

test-coverage-check:
	$(PYTHON) scripts/test_list_untested_headers.py
	$(PYTHON) scripts/list_untested_headers.py --require-complete

standalone-generator-interface-check:
	$(PYTHON) scripts/test_check_standalone_generator_interfaces.py
	$(PYTHON) scripts/check_standalone_generator_interfaces.py

standalone-assets-test: no-boost-dependency-check test-coverage-check test-verifier-markers standalone-generator-interface-check
	$(PYTHON) scripts/test_standalone_verification_results.py
	$(PYTHON) scripts/test_check_standalone_verification_results.py
	$(PYTHON) scripts/test_competitive_verifier_docs_result.py
	$(PYTHON) -S scripts/test_standalone_results_workflows.py
	$(PYTHON) scripts/test_run_standalone_assets.py
	$(PYTHON) scripts/test_prune_standalone_verification_results.py
	$(PYTHON) scripts/test_check_unsupported_onlinejudge_assets.py
	$(PYTHON) scripts/check_unsupported_onlinejudge_assets.py

standalone-results-prune:
	$(PYTHON) scripts/prune_standalone_verification_results.py \
		--result-dir $(STANDALONE_RESULT_DIR)

standalone-results-check:
	$(PYTHON) scripts/check_standalone_verification_results.py \
		--result-dir $(STANDALONE_RESULT_DIR)

standalone-assets: gcc13-check standalone-assets-test standalone-results-prune
	$(PYTHON) scripts/run_standalone_assets.py \
		--cache-dir $(STANDALONE_ASSET_CACHE) \
		--result-dir $(STANDALONE_RESULT_DIR) \
		--cxx "$(CXX)" \
		--cxxflags "$(CXXFLAGS)" \
		--split-size "$(STANDALONE_SPLIT_SIZE)" \
		--split-index "$(STANDALONE_SPLIT_INDEX)"

verify:
	@jobs="$(LOCAL_VERIFY_JOBS)"; \
	if ! [[ "$$jobs" =~ ^[1-9][0-9]*$$ ]]; then \
		echo "[verify] LOCAL_VERIFY_JOBS must be a positive integer" >&2; \
		exit 2; \
	fi; \
	resolve_status=0; \
	$(MAKE) --no-print-directory verifier-resolve || resolve_status=$$?; \
	standalone_status=0; \
	if $(MAKE) --no-print-directory standalone-assets-test; then \
		$(PYTHON) scripts/prune_standalone_verification_results.py \
			--result-dir $(STANDALONE_RESULT_DIR) || standalone_status=$$?; \
		if [ $$standalone_status -eq 0 ]; then \
			standalone_pids=(); \
			for ((index = 0; index < jobs; ++index)); do \
				$(PYTHON) scripts/run_standalone_assets.py \
					--cache-dir $(STANDALONE_ASSET_CACHE) \
					--result-dir $(STANDALONE_RESULT_DIR) \
					--cxx "$(CXX)" \
					--cxxflags "$(CXXFLAGS)" \
					--split-size "$$jobs" \
					--split-index "$$index" & \
				standalone_pids+=("$$!"); \
			done; \
			for pid in "$${standalone_pids[@]}"; do \
				wait "$$pid" || standalone_status=1; \
			done; \
			$(PYTHON) scripts/check_standalone_verification_results.py \
				--result-dir $(STANDALONE_RESULT_DIR) || standalone_status=1; \
		fi; \
	else \
		standalone_status=$$?; \
	fi; \
	verifier_status=0; \
	if [ $$resolve_status -ne 0 ]; then \
		verifier_status=$$resolve_status; \
		echo "[verify] competitive-verifier was not run because verifier-resolve failed" >&2; \
	elif $(VERIFIER_VENV)/bin/python scripts/test_run_competitive_verifier_shards.py; then \
		$(VERIFIER_COMMAND_ENV) $(VERIFIER_VENV)/bin/python scripts/run_competitive_verifier_shards.py \
			--verifier $(VERIFIER) \
			--verify-json $(VERIFY_FILES) \
			--output $(VERIFY_RESULT) \
			--prev-result $(VERIFY_RESULT) \
			--shard-dir $(VERIFIER_CACHE)/local-shards \
			--jobs "$$jobs" || verifier_status=$$?; \
	else \
		verifier_status=$$?; \
	fi; \
	if [ $$standalone_status -ne 0 ]; then \
		echo "[verify] standalone assets failed (exit $$standalone_status)" >&2; \
	fi; \
	if [ $$verifier_status -ne 0 ]; then \
		echo "[verify] competitive-verifier failed (exit $$verifier_status)" >&2; \
	fi; \
	if [ $$standalone_status -ne 0 ] || [ $$verifier_status -ne 0 ]; then \
		exit 1; \
	fi

docs-title-check: no-boost-dependency-check
	$(PYTHON) scripts/test_check_docs_bilingual_titles.py
	$(PYTHON) scripts/test_check_problem_independent_library_names.py
	$(PYTHON) scripts/test_docs_problem_tags.py
	$(PYTHON) scripts/test_docs_problem_tags_primary_sources.py
	$(PYTHON) scripts/test_docs_problem_tags_transitive_sources.py
	$(PYTHON) scripts/check_docs_bilingual_titles.py docs
	$(PYTHON) scripts/check_problem_independent_library_names.py src docs
	$(PYTHON) scripts/docs_problem_tags.py docs test/onlinejudge

docs-coverage-check:
	$(PYTHON) scripts/test_check_docs_markdown.py
	$(PYTHON) scripts/check_docs_markdown.py docs
	$(PYTHON) scripts/check_docs_coverage.py src docs

docs-source: docs-title-check docs-coverage-check docs-verifier-resolve
	$(PYTHON) scripts/test_competitive_verifier_docs_result.py
	$(PYTHON) scripts/competitive_verifier_docs_result.py \
		$(DOCS_VERIFY_FILES) $(VERIFY_RESULT) \
		--standalone-results $(STANDALONE_RESULT_DIR) > $(DOCS_RESULT)
	$(VERIFIER) docs \
		--verify-json $(DOCS_VERIFY_FILES) \
		--destination $(DOCS_SOURCE) \
		$(DOCS_RESULT)
	$(PYTHON) scripts/test_docs_standalone_index.py
	$(PYTHON) scripts/test_check_generated_docs_index.py
	$(PYTHON) scripts/test_reorder_competitive_verifier_index.py
	$(VERIFIER_VENV)/bin/python scripts/reorder_competitive_verifier_index.py \
		$(DOCS_SOURCE)/index.md
	$(VERIFIER_VENV)/bin/python scripts/check_generated_docs_index.py \
		$(DOCS_SOURCE)/index.md $(DOCS_VERIFY_FILES)


docs-prerequisites:
	@command -v ruby >/dev/null || { \
		echo "Rubyが必要です: sudo apt install ruby-all-dev ruby-bundler"; \
		exit 1; \
	}
	@command -v bundle >/dev/null || { \
		echo "Bundlerが必要です: sudo apt install ruby-bundler"; \
		exit 1; \
	}

docs: docs-prerequisites docs-source
	BUNDLE_GEMFILE=$(abspath $(DOCS_SOURCE)/Gemfile) \
	BUNDLE_PATH=$(abspath $(BUNDLE_PATH)) bundle check || \
	BUNDLE_GEMFILE=$(abspath $(DOCS_SOURCE)/Gemfile) \
	BUNDLE_PATH=$(abspath $(BUNDLE_PATH)) bundle install
	$(JEKYLL_URL_CONFIG_COMMAND)
	BUNDLE_GEMFILE=$(abspath $(DOCS_SOURCE)/Gemfile) \
	BUNDLE_PATH=$(abspath $(BUNDLE_PATH)) bundle exec jekyll build \
		--source $(abspath $(DOCS_SOURCE)) \
		--destination $(abspath $(DOCS_OUTPUT)) $(JEKYLL_BUILD_ARGS)
	@echo "Generated HTML: $(DOCS_OUTPUT)/index.html"

docs-serve: docs
	BUNDLE_GEMFILE=$(abspath $(DOCS_SOURCE)/Gemfile) \
	BUNDLE_PATH=$(abspath $(BUNDLE_PATH)) bundle exec jekyll serve \
		--source $(abspath $(DOCS_SOURCE)) \
		--destination $(abspath $(DOCS_OUTPUT)) \
		--host 127.0.0.1 --port 4000 $(JEKYLL_BUILD_ARGS)

verifier-clean:
	rm -rf $(VERIFIER_VENV) $(VERIFIER_CACHE) $(VERIFIER_ROOT)/bundled \
		$(DOCS_SOURCE) $(DOCS_OUTPUT) $(VERIFIER_ROOT)/vendor .sass-cache
