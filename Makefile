SHELL := /bin/bash

PYTHON ?= python3
COMPETITIVE_VERIFIER_VERSION ?= 4.1.2

VERIFIER_ROOT := .competitive-verifier
VERIFIER_VENV := $(VERIFIER_ROOT)/venv
VERIFIER := $(VERIFIER_VENV)/bin/competitive-verifier
VERIFIER_CACHE := $(VERIFIER_ROOT)/cache
VERIFY_FILES := $(VERIFIER_CACHE)/verify-files.json
VERIFY_RESULT := $(VERIFIER_CACHE)/result.json
DOCS_RESULT := $(VERIFIER_CACHE)/docs-result.json
DOCS_SOURCE := $(VERIFIER_ROOT)/_jekyll
DOCS_OUTPUT := $(VERIFIER_ROOT)/site
BUNDLE_PATH := $(VERIFIER_ROOT)/vendor/bundle
STANDALONE_ASSET_CACHE := $(VERIFIER_CACHE)/standalone-assets
CXX ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra

COMMA := ,
JEKYLL_URL_CONFIG := $(VERIFIER_CACHE)/jekyll-url.yml
JEKYLL_URL_CONFIG_COMMAND := $(if $(strip $(JEKYLL_URL)),$(PYTHON) scripts/write_jekyll_url_config.py --output "$(JEKYLL_URL_CONFIG)")
JEKYLL_BUILD_ARGS := $(strip \
	$(if $(strip $(JEKYLL_URL)),--config "$(abspath $(DOCS_SOURCE)/_config.yml)$(COMMA)$(abspath $(JEKYLL_URL_CONFIG))") \
	$(if $(strip $(JEKYLL_BASEURL)),--baseurl "$(JEKYLL_BASEURL)") \
)

.PHONY: help verifier-setup verifier-resolve standalone-assets verify docs-source docs-prerequisites docs docs-serve verifier-clean

help:
	@echo "make verify  competitive-verifierでtestを実行"
	@echo "make standalone-assets  standalone testのgenerator/checkerを実行"
	@echo "make docs    testを実行せずHTMLを$(DOCS_OUTPUT)へ生成"
	@echo "make docs-serve  HTMLを生成してlocalhost:4000で配信"
	@echo "make verifier-clean  ローカル生成物とvenvを削除"

$(VERIFIER):
	$(PYTHON) -m venv $(VERIFIER_VENV)
	$(VERIFIER_VENV)/bin/pip install "competitive-verifier==$(COMPETITIVE_VERIFIER_VERSION)"

verifier-setup: $(VERIFIER)

verifier-resolve: verifier-setup
	@mkdir -p $(VERIFIER_CACHE)
	$(VERIFIER) oj-resolve --include src test --config config.toml > $(VERIFY_FILES).tmp
	mv $(VERIFY_FILES).tmp $(VERIFY_FILES)

standalone-assets:
	$(PYTHON) scripts/run_standalone_assets.py \
		--cache-dir $(STANDALONE_ASSET_CACHE) \
		--cxx "$(CXX)" \
		--cxxflags "$(CXXFLAGS)"

verify:
	@standalone_status=0; \
	verifier_status=0; \
	$(PYTHON) scripts/run_standalone_assets.py \
		--cache-dir $(STANDALONE_ASSET_CACHE) \
		--cxx "$(CXX)" \
		--cxxflags "$(CXXFLAGS)" || standalone_status=$$?; \
	if $(MAKE) --no-print-directory verifier-resolve; then \
		$(VERIFIER) verify \
			--verify-json $(VERIFY_FILES) \
			--check-error \
			--output $(VERIFY_RESULT) || verifier_status=$$?; \
	else \
		verifier_status=$$?; \
		echo "[verify] competitive-verifier was not run because verifier-resolve failed" >&2; \
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

docs-source: verifier-resolve
	$(PYTHON) scripts/competitive_verifier_docs_result.py \
		$(VERIFY_FILES) $(VERIFY_RESULT) > $(DOCS_RESULT)
	$(VERIFIER) docs \
		--verify-json $(VERIFY_FILES) \
		--destination $(DOCS_SOURCE) \
		$(DOCS_RESULT)


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
