# ALP ch.3: Process

## Lý thuyết

Theo *Advanced Linux Programming*, chương 3 "Processes". Lab: `10-alp/02`, `07`, `08`, xem thêm `03-process/*`, `04-signal/*`. Chủ đề "Process, bộ nhớ ảo và lập lịch" và "Signal và IPC" ở trên có phần tổng quan.

### Xem process (3.1)
- Mỗi process có **PID** duy nhất (kiểu `pid_t`, `<sys/types.h>`) và **PPID** (PID của cha). `getpid()`, `getppid()`.
- `ps` (mặc định chỉ process của terminal hiện tại), `ps -e -o pid,ppid,command` (mọi process, chọn cột), `ps aux`, `ps -ef`. Process gốc là `init` (PID 1).
- `kill PID` gửi **SIGTERM** (mặc định); `kill -KILL PID` / `kill -9 PID` gửi SIGKILL. Từ C: `kill(pid, sig)` (`<signal.h>`).

### Tạo process (3.2)
- **`system("lệnh")`** (`<stdlib.h>`): chạy lệnh qua `/bin/sh -c`, chờ xong, trả về wait status (127 nếu shell không chạy được lệnh). Tiện nhưng **sách khuyên tránh**: phụ thuộc shell của hệ thống, chậm, và nguy hiểm khi chuỗi lệnh có input của người dùng (setuid program + system = lỗ hổng).
- **`fork()`**: tạo bản sao của process. Trả về **PID con trong cha**, **0 trong con**, -1 nếu lỗi. Con kế thừa bộ nhớ (copy-on-write), fd đang mở, môi trường, signal handler; khác PID, PPID, signal đang chờ, thời gian CPU.
- **Họ exec**: thay chương trình đang chạy bằng chương trình mới (cùng PID). Thành công thì **không bao giờ return**.
  - `p` (`execvp`, `execlp`): tìm chương trình theo `PATH`; không có `p` thì phải ghi đường dẫn đầy đủ.
  - `v` (`execv`, `execvp`): argv là mảng kết thúc bằng NULL.
  - `l` (`execl`, `execlp`, `execle`): đối số liệt kê trực tiếp, kết thúc bằng `NULL`.
  - `e` (`execve`, `execle`): truyền thêm mảng môi trường `"TEN=giatri"`.
  - Phần tử đầu của argv nên là tên chương trình.
- **Mẫu fork + exec** (listing 3.4 `spawn`): con gọi `execvp(program, arg_list)`; nếu exec trả về (lỗi) thì in lỗi và `abort()`.
- **Lập lịch**: Linux lập lịch độc lập cho cha và con, không đảm bảo ai chạy trước. **Niceness** từ -20 (ưu tiên cao nhất) tới 19, mặc định 0. `nice -n 10 prog`, `renice -n 10 -p PID`, hàm `nice(incr)`. Chỉ root mới giảm niceness (tăng ưu tiên).

### Signal (3.3)
- Signal là thông báo **bất đồng bộ** gửi tới process. Khi nhận, process làm một trong: hành động mặc định (kết thúc, bỏ qua, dừng, core dump), bỏ qua, hoặc gọi **handler**.
- Signal hay gặp: `SIGINT` (Ctrl+C), `SIGTERM` (yêu cầu kết thúc, mặc định của kill), `SIGKILL` (giết ngay, **không bắt, không bỏ qua được**), `SIGSTOP` (dừng, cũng không bắt được), `SIGHUP` (mất terminal, hay dùng để yêu cầu đọc lại cấu hình), `SIGUSR1`/`SIGUSR2` (tự định nghĩa), `SIGSEGV` (truy cập bộ nhớ sai), `SIGBUS` (truy cập bộ nhớ không hợp lệ, ví dụ vượt cuối file đã mmap), `SIGFPE` (lỗi số học, ví dụ chia nguyên cho 0), `SIGABRT` (abort), `SIGCHLD` (con kết thúc), `SIGPIPE` (ghi vào pipe không còn đầu đọc), `SIGALRM` (alarm).
- **`sigaction(sig, &new, &old)`**: cài hành vi. `struct sigaction` có `sa_handler` (hàm `void handler(int)`, hoặc `SIG_DFL` = mặc định, `SIG_IGN` = bỏ qua), `sa_mask` (signal bị chặn trong lúc chạy handler), `sa_flags` (`SA_RESTART`, `SA_SIGINFO`...). Sách khuyên dùng sigaction thay vì `signal()` (hành vi khác nhau giữa các hệ).
- Handler phải **làm thật ít việc** và return nhanh, vì nó ngắt chương trình ở điểm bất kỳ (kể cả giữa một lời gọi thư viện). Không gọi hàm không async-signal-safe như printf, malloc trong handler. Thường chỉ ghi lại rằng signal đã đến.
- Biến dùng chung giữa handler và chương trình nên có kiểu **`sig_atomic_t`** (gán được trong một lệnh, không bị ngắt giữa chừng), khai báo `volatile`. Ví dụ listing 3.5 đếm số lần nhận SIGUSR1.

### Kết thúc process (3.4)
- Kết thúc bình thường: return từ main hoặc gọi `exit(code)` (chạy các hàm `atexit`, flush stdio). `_exit` kết thúc ngay, không flush.
- Kết thúc bất thường do signal: SIGINT, SIGTERM, SIGKILL, SIGABRT (từ `abort()`), SIGSEGV...
- **Exit code** chỉ 8 bit; dùng 0–127. Bị signal giết thì shell báo 128 + số signal.
- **`wait(&status)`**: chặn tới khi **một con bất kỳ** kết thúc, trả về PID con đó. **`waitpid(pid, &status, options)`**: chờ con cụ thể (`-1` = bất kỳ); `WNOHANG` = không chặn (trả về 0 nếu chưa có con nào xong). `wait3`/`wait4`: trả thêm thống kê tài nguyên (`struct rusage`).
- Macro đọc status: `WIFEXITED(s)` + `WEXITSTATUS(s)` (thoát bình thường và exit code), `WIFSIGNALED(s)` + `WTERMSIG(s)` (bị signal giết, signal nào), `WIFSTOPPED`, `WCOREDUMP`.
- **Zombie**: con đã kết thúc nhưng cha chưa wait → vẫn giữ entry trong bảng process để cha lấy được exit status; `ps` hiện trạng thái `Z` / `<defunct>`. Không giết được zombie bằng kill (nó đã chết rồi); cha wait thì nó biến mất. Cha chết trước thì con (kể cả zombie) được **init** nhận nuôi và init sẽ dọn.
- **Dọn con bất đồng bộ** (listing 3.7): cài handler cho **SIGCHLD** và gọi `wait` trong handler (lưu status vào biến `sig_atomic_t`). Nhờ vậy cha không phải chặn chờ mà vẫn không để lại zombie. Nhiều con chết gần nhau có thể chỉ sinh một SIGCHLD → nên lặp `waitpid(-1, &s, WNOHANG)` tới khi trả về ≤ 0.

## Trắc nghiệm

### Hàm nào trả về PID của process cha?
- [ ] `getpid()`
- [x] `getppid()`
- [ ] `getuid()`
- [ ] `getpgid()`
> getpid() là PID của chính nó; getuid() là user ID; getpgid() là process group ID.

### `kill 1234` (không ghi signal) gửi signal nào?
- [ ] SIGKILL
- [x] SIGTERM
- [ ] SIGINT
- [ ] SIGHUP
> SIGTERM bắt được nên chương trình có cơ hội dọn dẹp. `kill -9`/`kill -KILL` mới là SIGKILL.

### Vì sao sách khuyên tránh `system()` trong chương trình thật? (chọn tất cả đáp án đúng)
- [x] Phụ thuộc vào shell `/bin/sh` của hệ thống, hành vi có thể khác nhau
- [x] Nguy hiểm khi chuỗi lệnh chứa dữ liệu từ người dùng (shell injection), nhất là trong chương trình setuid
- [x] Tốn thêm một process shell
- [ ] system() không trả về exit code
> system trả về wait status (đọc bằng WEXITSTATUS). Dùng fork + exec để kiểm soát chính xác chương trình và đối số.

### fork() trả về gì trong process con?
- [ ] PID của con
- [ ] PID của cha
- [x] 0
- [ ] -1
> Cha nhận PID của con (> 0). -1 nghĩa là fork thất bại (không có process con nào được tạo).

### Đoạn code in ra bao nhiêu dòng "hello"?
```c
fork();
fork();
printf("hello\n");
```
- [ ] 2
- [ ] 3
- [x] 4
- [ ] 8
> Mỗi fork nhân đôi số process: 1 → 2 → 4 process, mỗi process in một dòng.

### Process con sau fork KHÔNG kế thừa gì từ cha?
- [ ] Các file descriptor đang mở
- [ ] Biến môi trường
- [x] PID
- [ ] Signal handler đã cài
> Con có PID mới, PPID là PID của cha. Signal đang chờ (pending) và khoá file (fcntl) cũng không được kế thừa.

### Dòng `printf("after exec\n");` được in khi nào?
```c
execlp("ls", "ls", "-l", NULL);
printf("after exec\n");
```
- [ ] Luôn luôn, sau khi ls chạy xong
- [ ] Không bao giờ
- [x] Chỉ khi execlp thất bại
- [ ] Trước khi ls chạy
> exec thành công thay toàn bộ chương trình, không quay về. Vì vậy sau exec thường là code xử lý lỗi (listing 3.4 gọi abort).

### Hàm exec nào tìm chương trình theo biến PATH và nhận đối số dạng mảng?
- [ ] `execv`
- [x] `execvp`
- [ ] `execl`
- [ ] `execle`
> p = PATH, v = vector (mảng). execv cần đường dẫn đầy đủ; execl/execle nhận đối số liệt kê.

### Khi gọi `execl`, danh sách đối số phải kết thúc bằng gì?
- [ ] Chuỗi rỗng ""
- [x] Con trỏ NULL
- [ ] Số 0 kiểu int
- [ ] Không cần kết thúc
> Thường viết `(char *)NULL` để đúng kiểu trên mọi nền tảng. Mảng argv cho execv cũng phải kết thúc bằng NULL.

### Hàm exec nào cho phép truyền một môi trường hoàn toàn mới cho chương trình được chạy?
- [ ] `execvp`
- [ ] `execlp`
- [x] `execle` (và `execve`)
- [ ] `execv`
> Hậu tố e = environment. Các hàm khác dùng `environ` hiện tại của process.

### Sau fork, cha hay con chạy trước?
- [ ] Luôn là cha
- [ ] Luôn là con
- [x] Không xác định, do scheduler quyết định
- [ ] Con chạy xong cha mới chạy
> Cần thứ tự thì phải đồng bộ (wait, pipe, signal, semaphore).

### Giá trị niceness nào cho process ưu tiên CAO nhất?
- [x] -20
- [ ] 0
- [ ] 19
- [ ] 20
> Niceness càng cao càng "nhường" CPU. User thường chỉ tăng được niceness; giảm (tăng ưu tiên) cần root.

### Signal nào KHÔNG thể bắt, chặn hay bỏ qua? (chọn tất cả đáp án đúng)
- [x] SIGKILL
- [x] SIGSTOP
- [ ] SIGTERM
- [ ] SIGINT
> Đây là đường lui cuối cùng của hệ thống để luôn dừng hoặc giết được process.

### Ctrl+C trong terminal gửi signal nào tới process đang chạy foreground?
- [ ] SIGTERM
- [x] SIGINT
- [ ] SIGQUIT
- [ ] SIGTSTP
> Ctrl+\ gửi SIGQUIT (kèm core dump), Ctrl+Z gửi SIGTSTP (dừng process).

### Truy cập vượt quá cuối file trong vùng đã mmap thường gây signal nào?
- [ ] SIGSEGV
- [x] SIGBUS
- [ ] SIGFPE
- [ ] SIGPIPE
> SIGSEGV dùng cho địa chỉ không được map hoặc sai quyền; SIGBUS dùng cho địa chỉ có map nhưng không có trang dữ liệu thật phía sau.

### Phép chia số nguyên cho 0 trên Linux x86 gây signal nào?
- [ ] SIGSEGV
- [x] SIGFPE
- [ ] SIGABRT
- [ ] Không có signal, kết quả là 0
> FPE (floating-point exception) là tên lịch sử, nhưng signal này dùng cho mọi lỗi số học.

### Kiểu biến nào sách khuyên dùng cho biến dùng chung giữa signal handler và chương trình chính?
- [ ] `int`
- [ ] `atomic_int`
- [x] `sig_atomic_t` (kèm volatile)
- [ ] `pthread_mutex_t`
> Gán và đọc sig_atomic_t được thực hiện trong một lệnh máy nên không bị signal ngắt giữa chừng. Không dùng mutex trong handler (có thể deadlock).

### Vì sao không nên gọi printf hay malloc trong signal handler?
- [ ] Vì handler không có stack
- [x] Vì handler có thể ngắt đúng lúc chương trình đang ở trong printf/malloc; gọi lại sẽ làm hỏng trạng thái nội bộ (không async-signal-safe)
- [ ] Vì printf bị cấm trong mọi hàm static
- [ ] Vì printf luôn trả về EINTR
> Trong handler chỉ dùng các hàm async-signal-safe (write, _exit, ...), hoặc tốt nhất chỉ đặt một cờ rồi xử lý ở vòng lặp chính.

### Trong struct sigaction, gán `sa_handler = SIG_IGN` có nghĩa là?
- [ ] Khôi phục hành vi mặc định
- [x] Bỏ qua signal đó
- [ ] Chặn signal tạm thời cho tới khi bỏ chặn
- [ ] Gọi handler mặc định của thư viện
> SIG_DFL là hành vi mặc định. Chặn (block) bằng sigprocmask thì signal được giữ lại (pending) và giao sau, còn bỏ qua thì signal bị vứt đi.

### Khác nhau giữa `exit()` và `_exit()`?
- [ ] Không khác
- [x] exit() chạy các hàm atexit và flush buffer stdio rồi mới vào kernel; _exit() kết thúc ngay
- [ ] _exit() chỉ dùng cho thread
- [ ] exit() không đóng file descriptor
> Process con sau fork (khi exec thất bại) nên dùng `_exit`, để không flush lại buffer stdio đã copy từ cha (in trùng output).

### Con gọi `exit(258)`. Cha đọc `WEXITSTATUS(status)` được bao nhiêu?
- [ ] 258
- [x] 2
- [ ] 255
- [ ] 0
> Chỉ giữ 8 bit thấp: 258 mod 256 = 2.

### Con bị `abort()`. Macro nào đúng?
- [ ] WIFEXITED(status) đúng, WEXITSTATUS = 6
- [x] WIFSIGNALED(status) đúng, WTERMSIG = SIGABRT (6)
- [ ] WIFSTOPPED(status) đúng
- [ ] WEXITSTATUS = 134
> Bị signal giết thì không có exit code. 134 = 128 + 6 chỉ là cách shell báo trong `$?`.

### `waitpid(pid, &status, WNOHANG)` trả về 0 có nghĩa là?
- [ ] Con thoát với exit code 0
- [x] Con vẫn đang chạy, chưa có thay đổi trạng thái
- [ ] Lỗi, xem errno
- [ ] Không có con nào
> Không có con nào thì trả về -1 với errno = ECHILD.

### Zombie process là gì?
- [ ] Process có cha đã chết
- [x] Process đã kết thúc nhưng cha chưa gọi wait để lấy trạng thái thoát
- [ ] Process bị treo không phản hồi
- [ ] Process chạy nền sau khi đóng terminal
> Process có cha đã chết là orphan (được init nhận nuôi). Zombie không tốn CPU hay bộ nhớ, nhưng chiếm một entry/PID trong bảng process.

### Làm sao loại bỏ một zombie?
- [ ] `kill -9` zombie đó
- [x] Cho process cha gọi wait/waitpid, hoặc kết thúc process cha (init sẽ nhận nuôi và dọn zombie)
- [ ] Khởi động lại terminal
- [ ] `nice -n 19` zombie đó
> Zombie đã chết nên kill không có tác dụng.

### Cách nào giúp cha không phải chặn chờ mà vẫn không để lại zombie?
- [ ] Gọi sleep trong cha
- [x] Cài handler cho SIGCHLD và gọi wait/waitpid trong handler
- [ ] Gọi exit trong cha
- [ ] Chặn SIGCHLD
> Listing 3.7. Vì nhiều SIGCHLD có thể gộp thành một, nên lặp `while (waitpid(-1, &s, WNOHANG) > 0);`. Cũng có thể đặt SIGCHLD = SIG_IGN để kernel tự dọn (nhưng khi đó không lấy được exit status).

### `wait(&status)` khi process có 3 con đang chạy sẽ làm gì?
- [ ] Chờ cả 3 con kết thúc
- [x] Chặn tới khi một con bất kỳ kết thúc và trả về PID của con đó
- [ ] Trả về ngay -1
- [ ] Chờ con được tạo đầu tiên
> Muốn chờ hết thì gọi wait 3 lần (hoặc lặp tới khi trả về -1/ECHILD).
