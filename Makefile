# ─── Toolchain ────────────────────────────────────────────────────────────────
CC       := gcc
CXX      := g++

# Core objects: strict C99, no dynamic allocation, no exceptions, no STL.
CFLAGS   := -std=c99 -Wall -Wextra -Werror -pedantic -O2

# Test binaries: C++17 permitted (not safety-critical).
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

LDFLAGS  := -lm

# ─── Directory layout ─────────────────────────────────────────────────────────
#
#   core/        Safety-critical source (.c / .h) — no build artifacts
#   tests/       Test source (.cpp) — not safety-critical
#   build/       All generated files (.o, test binaries) — gitignored
#
BUILD := build
CORE  := core
TESTS := tests

# ─── Derived file lists ───────────────────────────────────────────────────────
CORE_SRCS := $(wildcard $(CORE)/*.c)
CORE_OBJS := $(patsubst $(CORE)/%.c, $(BUILD)/%.o, $(CORE_SRCS))

TEST_SRCS := $(wildcard $(TESTS)/*.cpp)
TEST_BINS := $(patsubst $(TESTS)/%.cpp, $(BUILD)/%, $(TEST_SRCS))

# ─── Phony targets ────────────────────────────────────────────────────────────
.PHONY: all test clean

# Default: compile core objects only (no test infrastructure).
all: $(CORE_OBJS)
	@echo "Core library ready: $(words $(CORE_OBJS)) object(s) in $(BUILD)/"

# Build and run every test binary; fail fast on the first suite failure.
test: $(TEST_BINS)
	@echo ""; \
	ok=0; fail=0; \
	for t in $(TEST_BINS); do \
	    echo ">>> $$(basename $$t)"; \
	    if $$t; then ok=$$((ok+1)); else fail=$$((fail+1)); fi; \
	    echo ""; \
	done; \
	echo "Suites: $$ok passed, $$fail failed."; \
	test $$fail -eq 0

# Remove all generated files; source tree is left untouched.
clean:
	rm -rf $(BUILD)

# ─── Build rules ──────────────────────────────────────────────────────────────

# Core .c → build/*.o
# -I$(CORE) lets each translation unit reach its sibling headers.
$(BUILD)/%.o: $(CORE)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(CORE) -c $< -o $@

# tests/*.cpp → build/<name>  (linked against all core objects)
$(BUILD)/%: $(TESTS)/%.cpp $(CORE_OBJS) | $(BUILD)
	$(CXX) $(CXXFLAGS) -I$(CORE) $^ -o $@ $(LDFLAGS)

# Create the build directory on demand.
$(BUILD):
	mkdir -p $@
