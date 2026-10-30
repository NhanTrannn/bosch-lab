# Top-level Makefile for bosch-lab
#   make                                  build every lab into build/<dir>/<lab>
#   make run T=linux-process/01_fork_count  build and run one lab
#   make check                            check data/source (question ids, answers, format)
#   make ids                              give new quiz questions a stable {#id}
#   make db                               rebuild data/bosch-lab.db now (the web server also does it on its own)
#   make test-web                         smoke-test the web UI in gjs (needs a running make web)
#   make clean
#   make web                              start the web UI at http://localhost:8000
# Lab sources live in data/source/labs/<dir>/; T and build paths use <dir>/<lab> without that prefix.
# No -O2 on purpose: the UB/volatile/race labs are easier to observe without it (see README).

CC       := gcc
CXX      := g++
CFLAGS   := -Wall -Wextra -g -std=c11
CXXFLAGS := -Wall -Wextra -g -std=c++17

LABS     := data/source/labs
DIRS     := c cpp linux-process linux-signal linux-thread linux-ipc alp design-patterns
HELPERS  := $(LABS)/c/04_helper.c
C_SRCS   := $(filter-out $(HELPERS),$(foreach d,$(DIRS),$(wildcard $(LABS)/$(d)/*.c)))
CPP_SRCS := $(foreach d,$(DIRS),$(wildcard $(LABS)/$(d)/*.cpp))
BINS     := $(patsubst $(LABS)/%.c,build/%,$(C_SRCS)) $(patsubst $(LABS)/%.cpp,build/%,$(CPP_SRCS))

all: $(BINS)

# Lab 04 is built from 2 files
build/c/04_static_extern: $(LABS)/c/04_static_extern.c $(LABS)/c/04_helper.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

# Thread labs need -pthread
build/linux-thread/%: $(LABS)/linux-thread/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -pthread $< -o $@

build/linux-thread/%: $(LABS)/linux-thread/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -pthread $< -o $@

# shm_open needs -lrt on glibc < 2.34 (Ubuntu 20.04); newer glibc ignores the flag
build/linux-ipc/%: $(LABS)/linux-ipc/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ -lrt

# Labs from the book Advanced Linux Programming: threads, dlopen, shm
build/alp/%: $(LABS)/alp/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -pthread $< -o $@ -ldl -lrt

# Design pattern labs: the singleton lab starts threads
build/design-patterns/%: $(LABS)/design-patterns/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -pthread $< -o $@

build/%: $(LABS)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

build/%: $(LABS)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@

run:
ifndef T
	$(error T is missing. Example: make run T=linux-process/01_fork_count)
endif
	@$(MAKE) --no-print-directory build/$(T)
	./build/$(T)

check:
	python3 tools/content.py check

ids:
	python3 tools/content.py ids

db:
	python3 web/backend/db.py

test-web:
	gjs -m tools/test_frontend.js $(CURDIR)/web/frontend

clean:
	rm -rf build

web:
	python3 web/backend/server.py

.PHONY: all run check ids db test-web clean web
