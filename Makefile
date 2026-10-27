# Makefile chung cho bosch-lab
#   make                               build tất cả vào build/
#   make run T=03-process/01_fork_count  build rồi chạy một bài
#   make clean
#   make web                           mở giao diện web ở http://localhost:8000
# Cố ý KHÔNG dùng -O2: các bài UB/volatile/race cho kết quả dễ quan sát hơn (xem README).

CC       := gcc
CXX      := g++
CFLAGS   := -Wall -Wextra -g -std=c11
CXXFLAGS := -Wall -Wextra -g -std=c++17

DIRS     := 01-c 02-cpp 03-process 04-signal 05-thread 06-ipc
HELPERS  := 01-c/04_helper.c
C_SRCS   := $(filter-out $(HELPERS),$(foreach d,$(DIRS),$(wildcard $(d)/*.c)))
CPP_SRCS := $(foreach d,$(DIRS),$(wildcard $(d)/*.cpp))
BINS     := $(patsubst %.c,build/%,$(C_SRCS)) $(patsubst %.cpp,build/%,$(CPP_SRCS))

all: $(BINS)

# Bài 04 build chung 2 file
build/01-c/04_static_extern: 01-c/04_static_extern.c 01-c/04_helper.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

# Bài thread cần -pthread
build/05-thread/%: 05-thread/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -pthread $< -o $@

build/05-thread/%: 05-thread/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -pthread $< -o $@

# shm_open cần -lrt trên glibc < 2.34 (Ubuntu 20.04); glibc mới bỏ qua cờ này
build/06-ipc/%: 06-ipc/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ -lrt

build/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

build/%: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@

run:
ifndef T
	$(error Thiếu T. Ví dụ: make run T=03-process/01_fork_count)
endif
	@$(MAKE) --no-print-directory build/$(T)
	./build/$(T)

clean:
	rm -rf build

web:
	python3 web/backend/server.py

.PHONY: all run clean web
