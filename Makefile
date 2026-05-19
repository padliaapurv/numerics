# ─── Toolchain ────────────────────────────────────────────────────────────────
CC       := gcc
CXX      := g++
PYTHON   := python3

# Core objects: strict C99, no dynamic allocation, no exceptions, no STL.
# -fPIC is required for shared library linkage; it is otherwise harmless.
CFLAGS   := -std=c99 -Wall -Wextra -Werror -pedantic -O2 -fPIC

# Test binaries: C++17 permitted (not safety-critical).
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

LDFLAGS  := -lm

# ─── Directory layout ─────────────────────────────────────────────────────────
#
#   core/        Safety-critical source (.c / .h) — no build artifacts
#   tests/       C++ test source — not safety-critical
#   python/      Python package and Python-level tests — not safety-critical
#   build/       All generated files (.o, binaries, .so) — gitignored
#
BUILD  := build
CORE   := core
TESTS  := tests
PYTHON_DIR := python

# ─── Derived file lists ───────────────────────────────────────────────────────
CORE_SRCS := $(wildcard $(CORE)/*.c)
CORE_OBJS := $(patsubst $(CORE)/%.c, $(BUILD)/%.o, $(CORE_SRCS))

TEST_SRCS := $(wildcard $(TESTS)/*.cpp)
TEST_BINS := $(patsubst $(TESTS)/%.cpp, $(BUILD)/%, $(TEST_SRCS))

LIB       := $(BUILD)/libNumerics.so

PYTHON_TESTS := $(wildcard $(PYTHON_DIR)/tests/test_*.py)

# ─── Phony targets ────────────────────────────────────────────────────────────
.PHONY: all lib test pytest testall clean

# Default: compile core objects only.
all: $(CORE_OBJS)
	@echo "Core objects ready ($(words $(CORE_OBJS))) in $(BUILD)/"

# Shared library consumed by the Python layer.
lib: $(LIB)

# C++ test suite.
test: $(TEST_BINS)
	@echo ""; \
	ok=0; fail=0; \
	for t in $(TEST_BINS); do \
	    echo ">>> $$(basename $$t)"; \
	    if $$t; then ok=$$((ok+1)); else fail=$$((fail+1)); fi; \
	    echo ""; \
	done; \
	echo "C++ suites: $$ok passed, $$fail failed."; \
	test $$fail -eq 0

# Python test suite (requires `make lib` first).
pytest: $(LIB)
	@echo ""; \
	ok=0; fail=0; \
	for f in $(PYTHON_TESTS); do \
	    echo ">>> $$(basename $$f)"; \
	    if $(PYTHON) $$f; then ok=$$((ok+1)); else fail=$$((fail+1)); fi; \
	    echo ""; \
	done; \
	echo "Python suites: $$ok passed, $$fail failed."; \
	test $$fail -eq 0

# Both suites together.
testall: test pytest

# Remove all generated files; source tree is left untouched.
clean:
	rm -rf $(BUILD)

# ─── Build rules ──────────────────────────────────────────────────────────────

# Core .c → build/*.o  (-fPIC in CFLAGS enables shared-library use)
$(BUILD)/%.o: $(CORE)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(CORE) -c $< -o $@

# Shared library for the Python layer.
$(LIB): $(CORE_OBJS) | $(BUILD)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

# tests/*.cpp → build/<name>
$(BUILD)/%: $(TESTS)/%.cpp $(CORE_OBJS) | $(BUILD)
	$(CXX) $(CXXFLAGS) -I$(CORE) $^ -o $@ $(LDFLAGS)

# Create build directory on demand.
$(BUILD):
	mkdir -p $@
