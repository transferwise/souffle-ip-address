GREEN  := $(shell tput -Txterm setaf 2)
YELLOW := $(shell tput -Txterm setaf 3)
WHITE  := $(shell tput -Txterm setaf 7)
RESET  := $(shell tput -Txterm sgr0)

.PHONY: all help devcontainer setup devenv lint fmt
.PHONY: prepare test

all: help

devcontainer: devcontainer-stamp

devcontainer-stamp: Dockerfile
	@docker build --tag transferwise/souffle-ip-address:devcontainer .
	touch $@

setup: setup-stamp

setup-stamp: devcontainer-stamp
	touch $@

devenv: devcontainer
	@DOCKER_ARGS="-it" ./scripts/devrun.sh bash

lint:
	./scripts/devrun.sh ./scripts/lint.sh

fmt:
	@echo pass, no fmt configured yet

libfunctors.so: datalog/ffi/functors.cpp
	./scripts/devrun.sh ./scripts/compile.sh $< $@

# Note: could be done better.
.PHONY: %.test
%.test: datalog/tests/%.dl datalog/tests/%.out libfunctors.so
	@./scripts/devrun.sh ./scripts/test.sh $<

test: ipv4.test

prepare: fmt test lint

help:
	@echo ''
	@echo 'Usage:'
	@echo '  ${YELLOW}make${RESET} ${GREEN}<target>${RESET}'
	@echo ''
	@echo 'Targets:'
	@echo "  ${YELLOW}setup            ${RESET} Build dev docker"
	@echo "  ${YELLOW}devenv           ${RESET} Start a devcontainer shell (run make ... there)"
	@echo "  ${YELLOW}fmt              ${RESET} Run formatters"
	@echo "  ${YELLOW}lint             ${RESET} Run linters"
	@echo "  ${YELLOW}test             ${RESET} Run all tests and checks"
	@echo "  ${YELLOW}prepare          ${RESET} Run fmt, test, lint"
