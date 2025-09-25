DEFAULT_GOAL := help

# Collects inspiration from https://github.com/itzmeanjan/ml-kem/blob/61cf680b1c0e2590bd7b650c07cd477e90cab46d/Makefile#L1-L8
.PHONY: help
help:
	@for file in $(MAKEFILE_LIST); do \
	   grep -E '^[a-zA-Z_-]+:.*?## .*$$' $${file} | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}';\
	done

CXX ?= clang++
CXX_FLAGS := -std=c++20
WARN_FLAGS := -Wall -Wextra -Wpedantic
DEBUG_FLAGS := -O1 -g
RELEASE_FLAGS := -O3 -march=native
LINK_OPT_FLAGS := -flto

I_FLAGS := -I ./include
SHA3_INC_DIR := ./sha3/include
RANDOMSHAKE_INC_DIR := ./RandomShake/include
DEP_IFLAGS := -I $(SHA3_INC_DIR) -I $(RANDOMSHAKE_INC_DIR)

SRC_DIR := include
ML_DSA_SOURCES := $(shell find $(SRC_DIR) -name '*.hpp')
BUILD_DIR := build

include tests/test.mk
include benchmarks/bench.mk
include examples/example.mk

$(RANDOMSHAKE_INC_DIR):
	git submodule update --init --recursive RandomShake

$(SHA3_INC_DIR): $(RANDOMSHAKE_INC_DIR)
	git submodule update --init sha3

$(GTEST_PARALLEL): $(SHA3_INC_DIR)
	git submodule update --init gtest-parallel

.PHONY: clean
clean: ## Remove build directory
	rm -rf $(BUILD_DIR)

.PHONY: format
format: $(ML_DSA_SOURCES) $(TEST_SOURCES) $(TEST_HEADERS) $(BENCHMARK_SOURCES) $(BENCHMARK_HEADERS) $(EXAMPLE_SOURCES) $(EXAMPLE_HEADERS) ## Format source code
	clang-format -i $^

.PHONY: sync_acvp_kats
sync_acvp_kats: ## Downloads NIST ACVP KAT vectors and updates local KATs
	cd kats/scripts && ./sync_acvp_kats.sh && cd -
