# Top-level Makefile for bosch-lab
#   make                               build everything into build/
#   make run T=03-process/01_fork_count  build and run one lab
#   make clean
#   make web                           start the web UI at http://localhost:8000
# No -O2 on purpose: the UB/volatile/race labs are easier to observe without it (see README).

CC       := gcc
CXX      := g++
CFLAGS   := -Wall -Wextra -g -std=c11
CXXFLAGS := -Wall -Wextra -g -std=c++17

DIRS     := 01-c 02-cpp 03-process 04-signal 05-thread 06-ipc 10-alp
HELPERS  := 01-c/04_helper.c
C_SRCS   := $(filter-out $(HELPERS),$(foreach d,$(DIRS),$(wildcard $(d)/*.c)))
CPP_SRCS := $(foreach d,$(DIRS),$(wildcard $(d)/*.cpp))
BINS     := $(patsubst %.c,build/%,$(C_SRCS)) $(patsubst %.cpp,build/%,$(CPP_SRCS))

all: $(BINS)

# Lab 04 is built from 2 files
build/01-c/04_static_extern: 01-c/04_static_extern.c 01-c/04_helper.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

# Thread labs need -pthread
build/05-thread/%: 05-thread/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -pthread $< -o $@

build/05-thread/%: 05-thread/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -pthread $< -o $@

# shm_open needs -lrt on glibc < 2.34 (Ubuntu 20.04); newer glibc ignores the flag
build/06-ipc/%: 06-ipc/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ -lrt

# Labs from the book Advanced Linux Programming: threads, dlopen, shm
build/10-alp/%: 10-alp/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -pthread $< -o $@ -ldl -lrt

build/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

build/%: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@

run:
ifndef T
	$(error T is missing. Example: make run T=03-process/01_fork_count)
endif
	@$(MAKE) --no-print-directory build/$(T)
	./build/$(T)

clean:
	rm -rf build

web:
	python3 web/backend/server.py

.PHONY: all run clean web
