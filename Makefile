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

verify: standalone-assets verifier-resolve
	$(VERIFIER) verify \
		--verify-json $(VERIFY_FILES) \
		--check-error \
		--output $(VERIFY_RESULT)

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
	BUNDLE_GEMFILE=$(abspath $(DOCS_SOURCE)/Gemfile) \
	BUNDLE_PATH=$(abspath $(BUNDLE_PATH)) bundle exec jekyll build \
		--source $(abspath $(DOCS_SOURCE)) \
		--destination $(abspath $(DOCS_OUTPUT))
	@echo "Generated HTML: $(DOCS_OUTPUT)/index.html"

docs-serve: docs
	BUNDLE_GEMFILE=$(abspath $(DOCS_SOURCE)/Gemfile) \
	BUNDLE_PATH=$(abspath $(BUNDLE_PATH)) bundle exec jekyll serve \
		--source $(abspath $(DOCS_SOURCE)) \
		--destination $(abspath $(DOCS_OUTPUT)) \
		--host 127.0.0.1 --port 4000

verifier-clean:
	rm -rf $(VERIFIER_VENV) $(VERIFIER_CACHE) $(VERIFIER_ROOT)/bundled \
		$(DOCS_SOURCE) $(DOCS_OUTPUT) $(VERIFIER_ROOT)/vendor .sass-cache
