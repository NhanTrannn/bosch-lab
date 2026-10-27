# Process, bộ nhớ ảo và lập lịch

## Lý thuyết

### Process và thread
- **Program**: file trên đĩa. **Process**: chương trình đang chạy, có không gian địa chỉ riêng, PID, bảng fd, thông tin trong **PCB** (Linux: `task_struct`).
- Trạng thái (`ps` cột STAT): **R** running/runnable, **S** sleep có thể ngắt, **D** sleep không ngắt được (thường chờ I/O), **T** stopped, **Z** zombie. Thêm: `+` foreground group, `s` session leader, `l` đa luồng, `<` ưu tiên cao, `N` ưu tiên thấp.
- Mô hình 5 trạng thái: New → Ready → Running → Waiting(Blocked) → Terminated.

### Tạo và kết thúc process
- `fork()`: trả 0 ở con, PID con ở cha, -1 khi lỗi. Con là bản sao (copy-on-write), kế thừa fd, signal handler, biến môi trường, cwd. n lần fork liên tiếp → 2ⁿ tiến trình.
- `exec*()`: thay chương trình trong tiến trình hiện tại, giữ PID và fd (trừ fd có `FD_CLOEXEC`). Chỉ trả về khi lỗi. Hậu tố: **l** danh sách đối số, **v** mảng, **p** tìm trong PATH, **e** truyền môi trường.
- `wait()/waitpid()`: chờ con kết thúc, lấy trạng thái (`WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`, `WTERMSIG`).
- `exit()` flush buffer stdio và chạy `atexit`; `_exit()` thoát ngay (dùng trong con sau fork).
- **Zombie**: con đã chết nhưng cha chưa `wait` → còn giữ entry trong bảng tiến trình (PID). **Orphan**: cha chết trước → con được init/systemd (hoặc subreaper) nhận nuôi.
- Mã thoát chỉ 8 bit (0–255); chết vì signal n → shell báo 128 + n.

### Bộ nhớ ảo
- Mỗi process có không gian địa chỉ ảo riêng; MMU dịch địa chỉ ảo → vật lý qua **page table**, cache bằng **TLB**. Trang thường 4 KiB.
- **Page fault**: truy cập trang chưa có trong RAM → kernel nạp (minor: trang đã ở RAM nhưng chưa map; major: phải đọc đĩa). Truy cập địa chỉ không hợp lệ → SIGSEGV.
- **Swap**: đẩy trang ít dùng ra đĩa. **Thrashing**: swap liên tục, hệ thống chậm hẳn.
- **Copy-on-write**: fork chia sẻ trang chỉ đọc, chỉ copy khi có ghi.
- Lợi ích: cô lập process, chương trình lớn hơn RAM, chia sẻ thư viện, mmap file.
- User space / kernel space: chương trình user vào kernel qua **system call** (chuyển chế độ CPU, ví dụ lệnh `syscall` trên x86_64). Ngắt và exception cũng chuyển vào kernel.
- Phân mảnh: **ngoài** (đủ tổng bộ nhớ trống nhưng không liên tục — paging giải quyết), **trong** (cấp phát thừa trong một khối/trang).

### Lập lịch (scheduling)
- Context switch: lưu thanh ghi, PC, (đổi page table nếu khác process) → tốn chi phí; chuyển giữa thread cùng process rẻ hơn.
- Thuật toán: FCFS, SJF (tối ưu thời gian chờ trung bình, cần biết trước), Round Robin (time slice), Priority (có thể starvation → aging), Multilevel feedback queue.
- Preemptive (hệ điều hành ngắt được tác vụ) vs non-preemptive (tác vụ tự nhường).
- Linux: CFS cho tác vụ thường (`nice` từ -20 đến 19, càng thấp càng ưu tiên); real-time `SCHED_FIFO`, `SCHED_RR` (ưu tiên 1–99) luôn chạy trước tác vụ thường. Linux 6.6+ thay CFS bằng EEVDF.
- **Priority inversion**: tác vụ ưu tiên cao chờ khoá do tác vụ thấp giữ, trong khi tác vụ trung bình chiếm CPU. Giải pháp: priority inheritance, priority ceiling (vụ Mars Pathfinder).

### Lệnh hữu ích
`ps -ef`, `ps aux`, `ps -o pid,ppid,stat,cmd`, `pstree -p`, `top`/`htop`, `kill`, `nice`/`renice`, `jobs`/`fg`/`bg`, `&`, `nohup`, `/proc/<pid>/` (status, maps, fd, cmdline), `strace` (xem system call), `free -h`, `vmstat`.

## Trắc nghiệm

### `fork()` trả về giá trị gì trong tiến trình con?
- [x] 0
- [ ] PID của con
- [ ] PID của cha
- [ ] -1
> Cha nhận PID của con, con nhận 0, lỗi trả -1 (ở cha, không có con nào được tạo).

### Đoạn sau in "hello" bao nhiêu lần?
```c
fork(); fork(); fork();
printf("hello\n");
```
- [ ] 3
- [ ] 6
- [x] 8
- [ ] 4
> Mỗi fork nhân đôi số tiến trình: 2³ = 8.

### Tiến trình zombie là gì?
- [ ] Tiến trình có cha đã chết
- [x] Tiến trình đã kết thúc nhưng cha chưa gọi wait() để lấy trạng thái
- [ ] Tiến trình bị treo vì deadlock
- [ ] Tiến trình đang ở trạng thái stopped
> Zombie (STAT Z, `<defunct>`) không dùng CPU/RAM nhưng giữ PID. kill -9 không xoá được; phải để cha wait hoặc cha chết.

### Khi cha chết trước con, con sẽ:
- [ ] Bị kill theo
- [ ] Trở thành zombie
- [x] Được init/systemd (hoặc tiến trình subreaper) nhận làm con
- [ ] Bị dừng lại (stopped)
> Đó là orphan process. PPID của nó đổi thành 1 hoặc PID của subreaper (ví dụ `systemd --user`).

### Sau khi gọi `execvp()` thành công, điều gì KHÔNG đổi?
- [ ] Code segment
- [ ] Heap
- [x] PID
- [ ] Stack
> exec thay toàn bộ image nhưng giữ PID, PPID, fd đang mở (trừ close-on-exec), cwd.

### Trạng thái D trong `ps` nghĩa là?
- [ ] Dead
- [x] Uninterruptible sleep (thường chờ I/O), không bị signal đánh thức
- [ ] Daemon
- [ ] Detached
> Tiến trình ở D không thể kill được cho tới khi I/O xong. Nhiều tiến trình D thường do đĩa/NFS có vấn đề.

### Copy-on-write trong fork() mang lại lợi ích gì?
- [ ] Cha và con dùng chung biến, thay đổi thấy được nhau
- [x] fork nhanh vì không copy bộ nhớ ngay, chỉ copy trang khi có ghi
- [ ] Ngăn con ghi vào bộ nhớ
- [ ] Giảm số PID cần dùng
> Đặc biệt có lợi cho fork + exec ngay sau đó (gần như không trang nào bị copy).

### Hàm nào nên dùng để thoát trong tiến trình con sau fork khi không muốn flush lại buffer stdio của cha?
- [ ] exit()
- [x] _exit()
- [ ] return
- [ ] abort()
> exit() flush buffer stdio đã kế thừa → output bị in hai lần. _exit() thoát ngay không flush.

### Mã thoát mà shell báo khi tiến trình bị kill bởi SIGKILL (9)?
- [ ] 9
- [ ] 1
- [x] 137
- [ ] 255
> 128 + số hiệu signal = 137. Với SIGINT (2) là 130, SIGSEGV (11) là 139.

### TLB là gì?
- [ ] Bảng lưu các tiến trình đang chạy
- [x] Bộ nhớ đệm phần cứng lưu các ánh xạ địa chỉ ảo → vật lý gần đây
- [ ] Bộ đệm của ổ cứng
- [ ] Bảng vector ngắt
> TLB miss buộc phải duyệt page table. Context switch giữa process có thể phải flush TLB (trừ khi có ASID/PCID).

### Page fault xảy ra khi nào?
- [ ] Khi chương trình chia cho 0
- [x] Khi truy cập trang ảo chưa được ánh xạ tới khung vật lý trong RAM
- [ ] Khi RAM bị lỗi phần cứng
- [ ] Khi stack bị tràn
> Không phải lỗi: kernel nạp trang và tiếp tục. Chỉ khi địa chỉ không hợp lệ mới gửi SIGSEGV.

### Thuật toán lập lịch nào có thể gây starvation?
- [ ] Round Robin
- [x] Priority scheduling không có aging
- [ ] FCFS
- [ ] Tất cả đều không
> Tác vụ ưu tiên thấp có thể chờ mãi. Aging tăng dần ưu tiên theo thời gian chờ. SJF cũng có thể làm job dài bị đói.

### Priority inversion là gì?
- [ ] Hệ điều hành đảo ngược ưu tiên của tất cả tác vụ
- [x] Tác vụ ưu tiên cao bị chặn bởi tác vụ ưu tiên thấp đang giữ tài nguyên, trong khi tác vụ ưu tiên trung bình chiếm CPU
- [ ] Tác vụ ưu tiên thấp chạy trước do FCFS
- [ ] Hai tác vụ cùng ưu tiên chờ nhau
> Giải pháp: priority inheritance (tác vụ giữ khoá tạm được nâng ưu tiên) hoặc priority ceiling.

### Giá trị `nice` nào cho ưu tiên cao nhất với tác vụ thường trên Linux?
- [x] -20
- [ ] 0
- [ ] 19
- [ ] 99
> nice từ -20 (ưu tiên cao) tới 19 (thấp). Chỉ root mới giảm nice xuống dưới 0. 1–99 là thang ưu tiên real-time.

### Context switch giữa hai thread cùng process rẻ hơn giữa hai process vì:
- [ ] Thread không có thanh ghi riêng
- [x] Không phải đổi không gian địa chỉ (page table) và ít bị flush TLB
- [ ] Thread không cần lưu PC
- [ ] Kernel không tham gia
> Vẫn phải lưu/khôi phục thanh ghi, stack pointer, PC.

### System call là gì?
- [ ] Hàm thư viện C chạy ở user space
- [x] Cơ chế để chương trình user yêu cầu kernel thực hiện dịch vụ, chuyển CPU sang kernel mode
- [ ] Lời gọi giữa hai process
- [ ] Ngắt phần cứng từ thiết bị
> Ví dụ read, write, open, fork. printf là hàm thư viện, bên trong gọi syscall write. Xem bằng `strace`.

### Lệnh nào hiển thị cây tiến trình kèm PID?
- [ ] ps -t
- [x] pstree -p
- [ ] top -tree
- [ ] ls /proc
> Cũng có thể dùng `ps -ef --forest` hoặc `ps axjf`.

### Nơi nào chứa thông tin runtime của tiến trình PID 1234 trên Linux?
- [ ] /var/run/1234
- [x] /proc/1234/
- [ ] /sys/process/1234
- [ ] /dev/1234
> /proc là filesystem ảo: status, maps (vùng nhớ), fd/ (fd đang mở), cmdline, environ.
