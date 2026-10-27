# Signal và IPC

## Lý thuyết

### Signal
- Thông báo bất đồng bộ gửi tới process. Gửi bằng `kill(pid, sig)`, `raise(sig)`, lệnh `kill -SIG pid`, phím (Ctrl+C → SIGINT, Ctrl+\ → SIGQUIT, Ctrl+Z → SIGTSTP), hoặc kernel (SIGSEGV, SIGPIPE, SIGCHLD...).
- Mỗi signal có hành động: mặc định (Term / Core / Ign / Stop / Cont), bỏ qua (`SIG_IGN`), hoặc bắt bằng handler.

| Signal | Số | Mặc định | Ghi chú |
| --- | --- | --- | --- |
| SIGHUP | 1 | Term | mất terminal; daemon hay dùng để đọc lại config |
| SIGINT | 2 | Term | Ctrl+C |
| SIGQUIT | 3 | Core | Ctrl+\ |
| SIGKILL | 9 | Term | **không bắt/chặn/bỏ qua được** |
| SIGSEGV | 11 | Core | truy cập bộ nhớ sai |
| SIGPIPE | 13 | Term | ghi vào pipe/socket không còn đầu đọc |
| SIGALRM | 14 | Term | `alarm()` |
| SIGTERM | 15 | Term | mặc định của lệnh `kill`, yêu cầu thoát "lịch sự" |
| SIGCHLD | 17 | Ign | con kết thúc/dừng |
| SIGCONT | 18 | Cont | |
| SIGSTOP | 19 | Stop | **không bắt/chặn/bỏ qua được** |
| SIGTSTP | 20 | Stop | Ctrl+Z, bắt được |

- `sigaction` nên dùng thay `signal` (hành vi rõ ràng, có `sa_mask`, `SA_RESTART`, `SA_SIGINFO`).
- Handler chỉ nên gọi hàm **async-signal-safe** (`write`, `_exit`...), **không** `printf`, `malloc`. Biến dùng chung với handler: `volatile sig_atomic_t`.
- Chặn (`sigprocmask`) → signal ở trạng thái pending. Signal chuẩn không xếp hàng (một bit pending); signal real-time (SIGRTMIN–SIGRTMAX) có xếp hàng.
- fork: con kế thừa handler và mask, pending bị xoá. exec: handler đặt lại về mặc định (signal bị ignore vẫn ignore).
- Syscall chậm bị ngắt bởi handler trả -1/EINTR, trừ khi có `SA_RESTART`.

### Các cơ chế IPC
| Cơ chế | Đặc điểm |
| --- | --- |
| **Pipe** (`pipe()`) | một chiều, luồng byte, chỉ giữa tiến trình họ hàng; dấu pipe trong shell |
| **FIFO / named pipe** (`mkfifo`) | như pipe nhưng có tên trong filesystem, tiến trình bất kỳ |
| **Message queue** (POSIX `mq_*`, System V `msg*`) | giữ ranh giới tin nhắn, có ưu tiên/kiểu |
| **Shared memory** (POSIX `shm_open`+`mmap`, System V `shmget`) | **nhanh nhất** (không copy qua kernel) nhưng phải tự đồng bộ (semaphore/mutex) |
| **Semaphore** (POSIX `sem_*`, System V `semget`) | đồng bộ, không truyền dữ liệu |
| **Socket** (UNIX domain / TCP / UDP) | hai chiều, có thể qua mạng |
| **Signal** | chỉ báo sự kiện, gần như không mang dữ liệu |
| **mmap file** | nhiều tiến trình map cùng file với `MAP_SHARED` |

- Pipe: `fd[0]` đọc, `fd[1]` ghi. Phải đóng đầu không dùng; còn đầu ghi mở thì read không bao giờ thấy EOF. Dung lượng mặc định 64 KiB trên Linux; ghi ≤ PIPE_BUF (4096) byte là atomic.
- Shell làm `a | b`: `pipe()` → fork hai lần → con 1 `dup2(fd[1], 1)`, con 2 `dup2(fd[0], 0)` → exec.
- Xem IPC System V: `ipcs`, xoá: `ipcrm`. POSIX: `/dev/shm`, `/dev/mqueue`.
- Socket: server `socket → bind → listen → accept → read/write`; client `socket → connect`. TCP tin cậy, có thứ tự, hướng kết nối; UDP không kết nối, giữ ranh giới datagram, có thể mất/đảo.

## Trắc nghiệm

### Signal nào KHÔNG thể bắt, chặn hay bỏ qua? (chọn tất cả đáp án đúng)
- [x] SIGKILL
- [ ] SIGTERM
- [x] SIGSTOP
- [ ] SIGINT
> SIGKILL (9) và SIGSTOP (19). SIGTERM, SIGINT, SIGTSTP đều bắt được.

### Lệnh `kill 1234` (không chỉ định signal) gửi signal nào?
- [ ] SIGKILL
- [x] SIGTERM
- [ ] SIGINT
- [ ] SIGHUP
> Mặc định là SIGTERM (15), cho chương trình cơ hội dọn dẹp. `kill -9` mới là SIGKILL.

### Nhấn Ctrl+C trong terminal gửi signal nào tới foreground process group?
- [x] SIGINT
- [ ] SIGTERM
- [ ] SIGQUIT
- [ ] SIGTSTP
> Ctrl+\ → SIGQUIT (tạo core dump), Ctrl+Z → SIGTSTP.

### Hàm nào an toàn để gọi trong signal handler?
- [ ] printf
- [ ] malloc
- [x] write
- [ ] fopen
> Chỉ các hàm async-signal-safe (danh sách trong `man 7 signal-safety`). printf và malloc dùng khoá/buffer nội bộ → có thể deadlock hoặc hỏng dữ liệu.

### Biến dùng chung giữa signal handler và chương trình chính nên khai báo kiểu gì?
- [ ] int
- [ ] static int
- [x] volatile sig_atomic_t
- [ ] atomic_flag
> volatile để compiler không cache, sig_atomic_t để đọc/ghi không bị ngắt giữa chừng.

### Signal SIGCHLD được gửi khi nào?
- [ ] Khi cha kết thúc
- [x] Khi một tiến trình con kết thúc (hoặc dừng/tiếp tục)
- [ ] Khi gọi fork
- [ ] Khi con gọi exec
> Hành động mặc định là bỏ qua. Bắt SIGCHLD và gọi waitpid(-1, ..., WNOHANG) trong vòng lặp để dọn zombie.

### Gửi SIGUSR1 năm lần trong khi nó đang bị chặn, sau đó bỏ chặn. Handler chạy mấy lần?
- [ ] 5
- [x] 1
- [ ] 0
- [ ] Không xác định
> Signal chuẩn chỉ có một bit pending, không xếp hàng. Signal real-time thì có xếp hàng.

### Khi tiến trình ghi vào pipe mà mọi đầu đọc đã đóng, nó nhận được gì?
- [ ] read trả 0
- [x] SIGPIPE (nếu bỏ qua SIGPIPE thì write trả -1, errno = EPIPE)
- [ ] write bị chặn mãi
- [ ] SIGCHLD
> Hành động mặc định của SIGPIPE là kết thúc tiến trình.

### Cơ chế IPC nào nhanh nhất cho lượng dữ liệu lớn giữa hai tiến trình trên cùng máy?
- [ ] Pipe
- [ ] Message queue
- [x] Shared memory
- [ ] Socket TCP
> Hai tiến trình truy cập trực tiếp cùng vùng nhớ, không copy qua kernel; đổi lại phải tự đồng bộ.

### Khác biệt chính giữa pipe và FIFO (named pipe)?
- [ ] FIFO là hai chiều, pipe một chiều
- [x] FIFO có tên trong filesystem nên tiến trình không có quan hệ họ hàng vẫn dùng được
- [ ] FIFO lưu dữ liệu xuống đĩa
- [ ] Pipe nhanh hơn rất nhiều
> FIFO tạo bằng `mkfifo`; `ls -l` hiện ký tự loại file `p`. Dữ liệu vẫn chỉ đi qua kernel, không ghi xuống đĩa.

### Sau `int fd[2]; pipe(fd);` đầu nào để đọc?
- [x] fd[0]
- [ ] fd[1]
- [ ] Cả hai
- [ ] Tuỳ hệ điều hành
> fd[0] đọc, fd[1] ghi (dễ nhớ: 0 là stdin, 1 là stdout).

### Vì sao tiến trình đọc pipe bị treo mãi thay vì nhận EOF?
- [ ] Vì pipe đầy
- [x] Vì vẫn còn một đầu ghi đang mở (thường là chính tiến trình đọc quên close(fd[1]))
- [ ] Vì chưa gọi fflush
- [ ] Vì thiếu SIGPIPE
> read chỉ trả 0 khi **mọi** fd ghi của pipe đã đóng, kể cả bản sao trong chính nó sau fork.

### `dup2(fd, 1)` làm gì?
- [ ] Đóng fd và mở lại ở số 1
- [x] Làm fd số 1 (stdout) trỏ tới cùng file với fd
- [ ] Copy nội dung file fd sang stdout
- [ ] Tạo pipe mới
> Shell dùng dup2 để làm redirect `>` và pipe `|`. `2>&1` tương ứng `dup2(1, 2)`.

### Đặc điểm của message queue so với pipe? (chọn tất cả đáp án đúng)
- [x] Giữ ranh giới giữa các tin nhắn
- [x] Hỗ trợ ưu tiên (POSIX) hoặc lọc theo kiểu (System V)
- [ ] Bắt buộc hai tiến trình phải họ hàng
- [x] Tồn tại trong kernel tới khi bị xoá, kể cả khi không còn tiến trình mở
> Pipe là luồng byte, mất khi mọi fd đóng.

### Socket kiểu nào giữ ranh giới tin nhắn và không bảo đảm thứ tự/đến nơi?
- [ ] SOCK_STREAM (TCP)
- [x] SOCK_DGRAM (UDP)
- [ ] UNIX domain stream
- [ ] Pipe
> TCP là luồng byte tin cậy, có thứ tự. UDP gửi datagram độc lập.

### Thứ tự gọi hàm ở phía server TCP?
- [ ] socket → connect → listen → accept
- [x] socket → bind → listen → accept
- [ ] bind → socket → accept → listen
- [ ] socket → listen → bind → connect
> Client: socket → connect. accept trả về fd mới cho từng kết nối.

### Lệnh nào liệt kê các object IPC System V (shm, semaphore, message queue)?
- [ ] lsof -i
- [x] ipcs
- [ ] ps -ipc
- [ ] netstat -x
> Xoá bằng `ipcrm`. (lsipc cũng có trên util-linux mới.) POSIX shm nằm trong /dev/shm.
