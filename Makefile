# ─── Toolchain ────────────────────────────────────────────────────────────────
CC      := gcc
CXX     := g++
PYTHON  := python3

# Core objects: strict C99, no dynamic allocation, no exceptions, no STL.
CFLAGS   := -std=c99 -Wall -Wextra -Werror -pedantic -O2 -fPIC
# Test binaries: C++17 permitted (not safety-critical).
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS  := -lm

# ─── Directory layout ─────────────────────────────────────────────────────────
#
#   src/                       Safety-critical C source
#     numerics.h / numerics.c  Shared config and status codes
#     primitives/              Scalar and vector operations (lowest layer)
#     linalg/                  Row ops, pivot, augmented, matrix ops
#     solvers/linear/          High-level solvers (Gauss-Jordan, …)
#   tests/
#     unit/                    One file per module
#     integration/             Full solver tests (correctness + stress)
#   python/                    Python package and Python-level tests
#   build/                     All generated files — gitignored
#
SRC        := src
PRIM       := $(SRC)/primitives
LINALG     := $(SRC)/linalg
LIN_SOLVE  := $(SRC)/solvers/linear
TESTS      := tests
PYTHON_DIR := python
BUILD      := build

# ─── Include flags (all source dirs on search path) ───────────────────────────
IFLAGS := -I$(SRC) -I$(PRIM) -I$(LINALG) -I$(LIN_SOLVE)

# ─── Source and object lists ──────────────────────────────────────────────────
CORE_SRCS := $(SRC)/numerics.c \
             $(wildcard $(PRIM)/*.c) \
             $(wildcard $(LINALG)/*.c) \
             $(wildcard $(LIN_SOLVE)/*.c)

# Mirror source tree under build/: src/linalg/mat.c → build/linalg/mat.o
CORE_OBJS := $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(CORE_SRCS))

UNIT_SRCS := $(wildcard $(TESTS)/unit/*.cpp)
INTG_SRCS := $(wildcard $(TESTS)/integration/*.cpp)
TEST_SRCS := $(UNIT_SRCS) $(INTG_SRCS)

UNIT_BINS := $(patsubst $(TESTS)/unit/%.cpp,        $(BUILD)/unit/%,        $(UNIT_SRCS))
INTG_BINS := $(patsubst $(TESTS)/integration/%.cpp, $(BUILD)/integration/%, $(INTG_SRCS))
TEST_BINS := $(UNIT_BINS) $(INTG_BINS)

LIB := $(BUILD)/libNumerics.so

PYTHON_TESTS := $(wildcard $(PYTHON_DIR)/tests/test_*.py)

# ─── Phony targets ────────────────────────────────────────────────────────────
.PHONY: all lib test unit integration pytest testall clean

all: $(CORE_OBJS)
	@echo "Core objects ready ($(words $(CORE_OBJS))) in $(BUILD)/"

lib: $(LIB)

unit: $(UNIT_BINS)
	@echo ""; ok=0; fail=0; \
	for t in $(UNIT_BINS); do \
	    echo ">>> $$(basename $$t)"; \
	    if $$t; then ok=$$((ok+1)); else fail=$$((fail+1)); fi; \
	    echo ""; \
	done; \
	echo "Unit suites: $$ok passed, $$fail failed."; \
	test $$fail -eq 0

integration: $(INTG_BINS)
	@echo ""; ok=0; fail=0; \
	for t in $(INTG_BINS); do \
	    echo ">>> $$(basename $$t)"; \
	    if $$t; then ok=$$((ok+1)); else fail=$$((fail+1)); fi; \
	    echo ""; \
	done; \
	echo "Integration suites: $$ok passed, $$fail failed."; \
	test $$fail -eq 0

test: unit integration

pytest: $(LIB)
	@echo ""; ok=0; fail=0; \
	for f in $(PYTHON_TESTS); do \
	    echo ">>> $$(basename $$f)"; \
	    if $(PYTHON) $$f; then ok=$$((ok+1)); else fail=$$((fail+1)); fi; \
	    echo ""; \
	done; \
	echo "Python suites: $$ok passed, $$fail failed."; \
	test $$fail -eq 0

testall: test pytest

clean:
	rm -rf $(BUILD)

# ─── Build rules ──────────────────────────────────────────────────────────────

# src/**/*.c → build/**/*.o  (subdirectory structure mirrored under build/)
$(BUILD)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

# Shared library for the Python layer.
$(LIB): $(CORE_OBJS)
	@mkdir -p $(@D)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

# Unit tests
$(BUILD)/unit/%: $(TESTS)/unit/%.cpp $(CORE_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(IFLAGS) -I$(TESTS)/unit $^ -o $@ $(LDFLAGS)

# Integration tests
$(BUILD)/integration/%: $(TESTS)/integration/%.cpp $(CORE_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(IFLAGS) $^ -o $@ $(LDFLAGS)
