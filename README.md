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

## Giao diện web (lab + lý thuyết + trắc nghiệm)
```sh
make web        # rồi mở http://localhost:8000
npm start       # tương đương, nếu đã cài Node/npm
```
Các lệnh npm khác: `npm run build` (= make), `npm run clean`, `npm run lab 03-process/01_fork_count` (= make run T=...).
- **Bài lab**: đoán output → build & chạy ngay trong trình duyệt (terminal thật: Ctrl+C/Ctrl+Z/stdin/kill/ps) → so sánh dự đoán với output thật → xem đáp án và tự chấm.
- **Lý thuyết**: tóm tắt theo 17 chủ đề, đọc từ `09-theory/*.md`.
- **Trắc nghiệm**: 411 câu có giải thích; chọn chủ đề, số câu, chế độ luyện tập/thi thử, làm lại câu sai.
- **Terminal 2** (góc dưới phải): bash thật cho các bài cần 2 terminal.
- Chỉ cần Python 3 có sẵn trên Ubuntu, không cài thêm gì. Tiến độ lưu trong trình duyệt.
- Cấu trúc frontend/backend và API: xem `web/README.md`.

## Bài cần chú ý
- Cần **2 terminal**: `03-process/05_zombie`, `04-signal/03_kill_test`, `06-ipc/03_fifo`.
- **Treo / chạy lâu cố ý** (nhấn **Ctrl+C** để thoát): `03-process/05_zombie`, `04-signal/01_sigint_handler`, `04-signal/03_kill_test`, `05-thread/03_deadlock`.
- `07-kernel-module` **không** nằm trong `make` ở gốc — xem `07-kernel-module/README.md`.
- `08-shell/drills.md`: 20 bài lệnh, không cần biên dịch.
- `04-signal/04_alarm_pause` cố ý chết vì SIGALRM ở cuối ("Alarm clock").
- `10-alp/05_errno_assert` cố ý chết vì assert sai ở cuối (SIGABRT, "Aborted").
- Ghi chú riêng (ví dụ chép từ khoá học Udemy): đặt vào `09-theory/private/*.md` cùng định dạng; thư mục này nằm trong `.gitignore`, chỉ hiện trên máy mình.
- Thêm câu hỏi trắc nghiệm: viết vào phần `## Trắc nghiệm` của file trong `09-theory/` theo mẫu `### câu hỏi` / `- [ ]` sai / `- [x]` đúng / `> giải thích`.

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
- `11_string_array_pointer.c` — Chuỗi: mảng char và con trỏ tới string literal
- `12_precedence.c` — Độ ưu tiên toán tử và short-circuit
- `13_function_pointer.c` — Con trỏ hàm, mảng con trỏ hàm và callback (qsort)
- `14_integer_promotion.c` — Integer promotion: char/short tự lên int
- `15_float.c` — Số thực: so sánh, làm tròn, chia nguyên
- `16_scope_storage.c` — Phạm vi biến, shadowing và static trong vòng lặp

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
- `10_virtual_default_arg.cpp` — Tham số mặc định của hàm virtual
- `11_exception_raii.cpp` — Exception, stack unwinding và RAII
- `12_diamond.cpp` — Đa kế thừa, diamond và virtual inheritance
- `13_static_member.cpp` — static member, static local và singleton
- `14_move_noexcept.cpp` — std::move, const và noexcept khi vector cấp phát lại
- `15_sizeof_class.cpp` — sizeof của class: class rỗng, vptr, padding
- `16_templates.cpp` — Template: suy luận kiểu, specialization, overload

Theo khoá Udemy *Beginning C++ Programming* (section 6–23):
- `17_types_conversion.cpp` — Biến, kiểu cơ bản, chia nguyên và chuyển kiểu
- `18_operators.cpp` — Toán tử: ++/--, gán kép, so sánh, logic và short-circuit
- `19_control_flow.cpp` — range-for copy/ref, do-while, continue/break, dangling else, ?:
- `20_strings.cpp` — C-style string và std::string
- `21_functions.cpp` — Tham trị/tham chiếu/con trỏ, tham số mặc định, overload, static local, đệ quy
- `22_pointers_refs.cpp` — Con trỏ và tham chiếu
- `23_class_basics.cpp` — Constructor, delegating constructor, copy constructor, const, static, shallow/deep copy
- `24_operator_overloading.cpp` — Operator overloading
- `25_inheritance.cpp` — Kế thừa: constructor lớp cha, redefine và virtual, abstract class
- `26_exceptions.cpp` — Exception: thứ tự catch, catch(...), rethrow, exception tự định nghĩa
- `27_streams.cpp` — iomanip, manipulator dính/không dính, stringstream, trạng thái lỗi
- `28_stl_algorithms.cpp` — Iterator, thuật toán, set/map, stack/queue/priority_queue
- `29_lambda_enum.cpp` — Lambda và enum / enum class

### 03-process
- `01_fork_count.c` — fork() ba lần
- `02_fork_if.c` — fork() trong if và trong biểu thức logic
- `03_fork_buffer.c` — Buffer của stdio khi fork
- `04_fork_exec_wait.c` — fork + exec + waitpid
- `05_zombie.c` — Tiến trình zombie
- `06_orphan.c` — Tiến trình mồ côi (orphan)
- `07_fork_cow.c` — Biến sau fork(): copy-on-write
- `08_fork_fd_offset.c` — File descriptor sau fork(): dùng chung offset
- `09_exec_buffer.c` — exec(): PID giữ nguyên, buffer stdio mất

### 04-signal
- `01_sigint_handler.c` — Bắt Ctrl+C bằng sigaction
- `02_sigchld_reap.c` — Dọn zombie bằng SIGCHLD
- `03_kill_test.c` — Gửi signal bằng lệnh kill
- `04_alarm_pause.c` — alarm, pause và hành động mặc định của SIGALRM
- `05_sigprocmask.c` — Chặn signal, pending và signal không xếp hàng

### 05-thread
- `01_race_condition.c` — Race condition với counter++
- `02_mutex_fix.c` — Sửa race bằng pthread_mutex
- `03_deadlock.c` — Deadlock do khoá ngược thứ tự
- `04_cond_var.c` — Producer–consumer một slot với condition variable
- `05_semaphore.c` — Semaphore giới hạn 2 thread cùng lúc
- `06_cpp_thread.cpp` — std::thread, std::mutex, lock_guard, std::atomic
- `07_thread_arg_bug.c` — Truyền &i vào thread và lấy kết quả qua pthread_join
- `08_thread_local.c` — Thread chia sẻ gì, riêng gì: global, thread-local, PID/TID

### 06-ipc
- `01_pipe.c` — pipe: cha ghi, con đọc
- `02_pipe_dup2.c` — Mô phỏng ls / | wc -l bằng dup2
- `03_fifo_reader.c + 03_fifo_writer.c` — Named pipe (FIFO) giữa 2 chương trình
- `04_shm.c` — POSIX shared memory giữa cha và con
- `05_mmap_file.c` — Ánh xạ file vào bộ nhớ bằng mmap
- `06_unix_socket.c` — UNIX domain socket: server/client bằng fork
- `07_mqueue.c` — POSIX message queue và độ ưu tiên
- `08_sigpipe.c` — Ghi vào pipe không còn đầu đọc: SIGPIPE và EPIPE

### 07-kernel-module
- `hello.c + Makefile` — Kernel module Hello

### 08-shell
- `drills.md` — 20 bài tập lệnh

### 10-alp (sách *Advanced Linux Programming*, chương 1–5)
- `01_getopt_long.c` — getopt_long: tùy chọn ngắn/dài, optarg, optind, `--`, hoán vị argv (2.1.3)
- `02_env_exit_code.c` — getenv/setenv/environ, exit code và 128 + signal (2.1.5, 2.1.6)
- `03_stdio_buffer.c` — Buffer full/line/none, stderr không buffer, exit và _exit (2.1.4)
- `04_tempfile.c` — mkstemp + unlink, tmpfile (2.1.7)
- `05_errno_assert.c` — errno, strerror, perror, assert và NDEBUG (2.2)
- `06_dlopen.c` — dlopen, dlsym, dlerror (2.3.6)
- `07_fork_exec.c` — system, fork + họ exec, spawn, nice (3.2)
- `08_wait_status.c` — wait/waitpid, WIFEXITED/WIFSIGNALED, WNOHANG, zombie (3.4)
- `09_thread_return.c` — Truyền dữ liệu, join lấy giá trị trả về, pthread_exit, detached (4.1)
- `10_thread_cancel.c` — Huỷ thread, vùng không huỷ được, cleanup handler (4.2, 4.3)
- `11_tsd_mutex.c` — Thread-specific data, mutex fast/recursive/error-checking, trylock, semaphore (4.3, 4.4)
- `12_sysv_shm_sem.c` — System V shared memory và semaphore, SEM_UNDO (5.1, 5.2)
- `13_mmap_shared_private.c` — mmap MAP_SHARED/MAP_PRIVATE, msync (5.3)
- `14_popen_fdopen.c` — pipe + fdopen, dup2 + exec sort, popen/pclose (5.4)
- `15_sockets.c` — socketpair, socket Internet qua 127.0.0.1, byte order (5.5)

### 09-theory (lý thuyết + trắc nghiệm)
- `01-c.md` · `02-cpp-oop.md` · `03-cpp-stl-modern.md` · `04-process-memory.md` · `05-signal-ipc.md` · `06-thread-sync.md` · `07-linux-shell-fs.md` · `08-build-debug.md` · `09-kernel-embedded.md`
- Theo khoá Udemy: `10-cpp-basics.md` · `11-cpp-functions-pointers.md` · `12-cpp-classes-operators.md` · `13-cpp-exceptions-io-stl.md`
- Theo sách Advanced Linux Programming: `14-alp-ch1-2-gnu-tools.md` · `15-alp-ch3-processes.md` · `16-alp-ch4-threads.md` · `17-alp-ch5-ipc.md`
