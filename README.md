# bosch-lab — Lab thực hành C/C++ & Linux System

Ôn assessment **C++ và Linux System Knowledge** (thứ Năm 29/10/2026). Mỗi bài là một file ngắn, chạy độc lập, comment tiếng Việt.
**Đoán output trước khi chạy**, rồi mới so với `ANSWERS.md` (hoặc trang Lab của website Bosch Prep).

> Thư mục này được sinh tự động từ website bằng `node tools/export-lab.js`. Sửa nội dung ở `assets/js/data/labs.js` rồi chạy lại script.

## Cài đặt (Ubuntu)
```sh
sudo apt install build-essential gdb strace ltrace valgrind
```
VS Code: cài extension **C/C++** (Microsoft).

## Build và chạy
```sh
make                                  # build tất cả vào build/
make run T=03-process/01_fork_count   # build rồi chạy một bài
make clean
```
- Cờ: `-Wall -Wextra -g`, C dùng `-std=c11`, C++ dùng `-std=c++17`, bài thread thêm `-pthread`.
- **Cố ý không dùng `-O2`**: các bài undefined behavior / volatile / race condition cho kết quả dễ quan sát và gần với đáp án hơn. Bật `-O2` có thể làm kết quả khác đi (đó cũng là bài học).
- Một số bài có **cảnh báo biên dịch cố ý** (ghi rõ trong comment đầu file, dòng "Cảnh báo biên dịch cố ý").
- Code POSIX có `#define _GNU_SOURCE` ở đầu vì `-std=c11` (strict) ẩn các hàm như `sigaction`, `kill`.
- VS Code: mở file bài → **Ctrl+Shift+B** (task "Build & run current file"); **F5** để debug bằng gdb (đặt breakpoint trước).

## Bài cần chú ý
- Cần **2 terminal**: `03-process/05_zombie`, `04-signal/03_kill_test`, `06-ipc/03_fifo`.
- **Treo / chạy lâu cố ý** (nhấn **Ctrl+C** để thoát): `03-process/05_zombie`, `04-signal/01_sigint_handler`, `04-signal/03_kill_test`, `05-thread/03_deadlock`.
- `07-kernel-module` **không** nằm trong `make` ở gốc — xem `07-kernel-module/README.md`.
- `08-shell/drills.md`: 20 bài lệnh, không cần biên dịch.

## Lịch học đề xuất
| Ngày | Thư mục |
| --- | --- |
| Thứ Hai 26/10 | `01-c`, `02-cpp` |
| Thứ Ba 27/10 | `03-process`, `04-signal`, `08-shell` |
| Thứ Tư 28/10 | `05-thread`, `06-ipc`, `07-kernel-module` |
| Thứ Năm 29/10 (sáng) | Chỉ đọc lại `ANSWERS.md`, làm lại các bài từng đoán sai |

## Danh sách bài
### 01-c
- `01_sizeof_padding.c` — sizeof và padding của struct
- `02_pointer_array.c` — Pointer arithmetic, mảng và decay
- `03_const_pointer.c` — Ba kiểu const với con trỏ
- `04_static_extern.c + 04_helper.c` — static local, static mức file và extern
- `05_bitwise.c` — Thao tác bit
- `06_endian_union.c` — Endianness bằng pointer và union
- `07_signed_unsigned.c` — Bẫy signed/unsigned và chia nguyên
- `08_macro_traps.c` — Bẫy macro
- `09_memory_layout.c` — Memory layout: text, data, bss, heap, stack
- `10_switch_fallthrough.c` — switch thiếu break

### 02-cpp
- `01_ctor_dtor_order.cpp` — Thứ tự constructor / destructor
- `02_virtual_slicing.cpp` — virtual, gọi qua object/pointer/reference và object slicing
- `03_virtual_dtor.cpp` — Destructor virtual và không virtual
- `04_virtual_in_ctor.cpp` — Gọi hàm virtual trong constructor/destructor
- `05_rule_of_five.cpp` — Rule of five: copy/move ctor và assignment
- `06_smart_ptr.cpp` — unique_ptr, shared_ptr, weak_ptr
- `07_lambda_capture.cpp` — Lambda capture theo giá trị và tham chiếu
- `08_stl_traps.cpp` — Bẫy STL: map[], erase trong vòng lặp, iterator invalidation
- `09_name_hiding.cpp` — Name hiding và using Base::f

### 03-process
- `01_fork_count.c` — fork() ba lần
- `02_fork_if.c` — fork() trong if và trong biểu thức logic
- `03_fork_buffer.c` — Buffer của stdio khi fork
- `04_fork_exec_wait.c` — fork + exec + waitpid
- `05_zombie.c` — Tiến trình zombie
- `06_orphan.c` — Tiến trình mồ côi (orphan)

### 04-signal
- `01_sigint_handler.c` — Bắt Ctrl+C bằng sigaction
- `02_sigchld_reap.c` — Dọn zombie bằng SIGCHLD
- `03_kill_test.c` — Gửi signal bằng lệnh kill

### 05-thread
- `01_race_condition.c` — Race condition với counter++
- `02_mutex_fix.c` — Sửa race bằng pthread_mutex
- `03_deadlock.c` — Deadlock do khoá ngược thứ tự
- `04_cond_var.c` — Producer–consumer một slot với condition variable
- `05_semaphore.c` — Semaphore giới hạn 2 thread cùng lúc
- `06_cpp_thread.cpp` — std::thread, std::mutex, lock_guard, std::atomic

### 06-ipc
- `01_pipe.c` — pipe: cha ghi, con đọc
- `02_pipe_dup2.c` — Mô phỏng ls / | wc -l bằng dup2
- `03_fifo_reader.c + 03_fifo_writer.c` — Named pipe (FIFO) giữa 2 chương trình
- `04_shm.c` — POSIX shared memory giữa cha và con
- `05_mmap_file.c` — Ánh xạ file vào bộ nhớ bằng mmap
- `06_unix_socket.c` — UNIX domain socket: server/client bằng fork

### 07-kernel-module
- `hello.c + Makefile` — Kernel module Hello

### 08-shell
- `drills.md` — 20 bài tập lệnh
