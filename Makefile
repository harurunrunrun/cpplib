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
DOCS_OUTPUT := $(VERIFIER_ROOT)/_jekyll

.PHONY: help verifier-setup verifier-resolve verify docs verifier-clean

help:
	@echo "make verify  competitive-verifierでtestを実行"
	@echo "make docs    testを実行せずdocsを$(DOCS_OUTPUT)へ生成"
	@echo "make verifier-clean  ローカル生成物とvenvを削除"

$(VERIFIER):
	$(PYTHON) -m venv $(VERIFIER_VENV)
	$(VERIFIER_VENV)/bin/pip install "competitive-verifier==$(COMPETITIVE_VERIFIER_VERSION)"

verifier-setup: $(VERIFIER)

verifier-resolve: verifier-setup
	@mkdir -p $(VERIFIER_CACHE)
	$(VERIFIER) oj-resolve --include src test --config config.toml > $(VERIFY_FILES).tmp
	mv $(VERIFY_FILES).tmp $(VERIFY_FILES)

verify: verifier-resolve
	$(VERIFIER) verify \
		--verify-json $(VERIFY_FILES) \
		--check-error \
		--output $(VERIFY_RESULT)

docs: verifier-resolve
	$(PYTHON) scripts/competitive_verifier_docs_result.py \
		$(VERIFY_FILES) > $(DOCS_RESULT)
	$(VERIFIER) docs \
		--verify-json $(VERIFY_FILES) \
		--destination $(DOCS_OUTPUT) \
		$(DOCS_RESULT)

verifier-clean:
	rm -rf $(VERIFIER_VENV) $(VERIFIER_CACHE) $(VERIFIER_ROOT)/bundled $(DOCS_OUTPUT)
