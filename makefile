# Single, non-recursive Makefile. Run from this directory.
#
#   make                                     build every .c (debug by default)
#   make containers/vector.c                 build just that file
#   make MODE=release containers/vector.c misc/scratch.c
#   make MODE=sanitize containers/deque.c
#   make MODE=release asm                    dump assembly for every .c
#   make MODE=release asm containers/vector.c
#   make clean
#
# Each .c is its own program with its own main().
# Output mirrors the source tree:
#   containers/vector.c -> build/<mode>/containers/vector
#                       -> asm/<mode>/containers/vector.s

MODE ?= debug

# c2x is the pre-release name for C23; newer GCC/Clang also accept -std=c23.
CSTD := -std=c2x
WARN := -Wall -Wextra -Wpedantic -Wshadow -Wconversion

ifeq ($(MODE),release)
  MODE_CFLAGS  := -O2 -DNDEBUG
else ifeq ($(MODE),debug)
  MODE_CFLAGS  := -O0 -g3
else ifeq ($(MODE),sanitize)
  MODE_CFLAGS  := -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
  MODE_LDFLAGS := -fsanitize=address,undefined
else
  $(error Unknown MODE '$(MODE)', expected release, debug or sanitize)
endif

CFLAGS  := $(CSTD) $(WARN) $(MODE_CFLAGS)
LDFLAGS := $(MODE_LDFLAGS)

# For readable assembly: strip -g (debug directives clutter the output),
# drop .cfi unwind directives, and use Intel syntax (remove for AT&T).
ASM_CFLAGS := $(filter-out -g%,$(CFLAGS)) -fno-asynchronous-unwind-tables -masm=intel

BUILD_DIR := build/$(MODE)
ASM_DIR   := asm/$(MODE)

# Turn off make's built-in implicit rules so they can't interfere.
MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

# ---- Which sources to build -------------------------------------------------

# Every .c under this directory, excluding the output folders, without "./".
ALL_SRCS := $(patsubst ./%,%,$(shell find . -name '*.c' \
                -not -path './build/*' -not -path './asm/*'))

# Any .c files named on the command line.
SRC_GOALS := $(patsubst ./%,%,$(filter %.c,$(MAKECMDGOALS)))

$(foreach src,$(SRC_GOALS),$(if $(wildcard $(src)),,$(error No such file: $(src))))

# Build only the named files if any were given, otherwise everything.
SRCS := $(or $(SRC_GOALS),$(ALL_SRCS))
BINS := $(SRCS:%.c=$(BUILD_DIR)/%)
ASMS := $(SRCS:%.c=$(ASM_DIR)/%.s)

# ---- Targets ----------------------------------------------------------------

.PHONY: all asm clean $(SRC_GOALS)

all: $(BINS)
asm: $(ASMS)

# A .c named as a goal already exists, so it's marked phony and made to
# depend on its output: the binary normally, or the .s if "asm" was asked for.
# The build rules below take the source by absolute path ($(CURDIR)/...), so
# make treats it as a different target from the phony goal, avoiding a
# goal -> binary -> goal dependency cycle.
ifneq ($(filter asm,$(MAKECMDGOALS)),)
$(SRC_GOALS): %.c: $(ASM_DIR)/%.s ; @:
else
$(SRC_GOALS): %.c: $(BUILD_DIR)/% ; @:
endif

$(BUILD_DIR)/%: $(CURDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(ASM_DIR)/%.s: $(CURDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(ASM_CFLAGS) -S $< -o $@

clean:
	rm -rf build asm
