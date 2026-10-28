# ALP ch.5: Giao tiếp giữa các process (IPC)

## Lý thuyết

Theo *Advanced Linux Programming*, chương 5 "Interprocess Communication". Lab: `10-alp/12` … `10-alp/15`, xem thêm `06-ipc/*`. Chủ đề "Signal và IPC" ở trên có bảng so sánh các cơ chế.

Sách bàn năm loại IPC: **shared memory**, **mapped memory**, **pipe**, **FIFO**, **socket**, cùng với **process semaphore** để đồng bộ. Ba tiêu chí phân loại:
- Có giới hạn giữa các process **có quan hệ họ hàng** (cha/con) không (pipe thì có).
- Chỉ đọc hoặc chỉ ghi, hay cả hai chiều.
- Số process có thể tham gia.

### Shared memory, System V (5.1)
- IPC **nhanh nhất**: các process truy cập cùng vùng nhớ vật lý, đọc ghi không cần system call hay copy. Nhưng kernel **không đồng bộ** gì cả → phải tự dùng semaphore/khoá.
- Mô hình: một process **cấp phát** segment, các process khác **attach**. Segment là bội số của **page size** (`getpagesize()`, thường 4 KB).
- `shmget(key, size, flags)`: tạo hoặc lấy segment. key = `IPC_PRIVATE` → luôn tạo segment mới. flags: `IPC_CREAT` (tạo nếu chưa có), `IPC_EXCL` (cùng IPC_CREAT: lỗi nếu đã tồn tại), quyền `S_IRUSR | S_IWUSR`. Trả về **segment id**.
- `shmat(id, addr, flags)`: gắn vào không gian địa chỉ (addr = NULL để kernel chọn), trả về địa chỉ (`SHM_RDONLY` = chỉ đọc). fork thì con kế thừa các segment đã gắn. `shmdt(addr)`: gỡ. exit/exec tự gỡ.
- `shmctl(id, IPC_STAT, &ds)`: thông tin (`shm_segsz`, `shm_nattch` = số lần đang được gắn). `shmctl(id, IPC_RMID, NULL)`: đánh dấu xoá; segment bị huỷ khi process cuối cùng gỡ ra.
- Segment **không tự mất** khi mọi process đã thoát. Phải IPC_RMID, nếu không sẽ rò rỉ cho tới khi reboot. Lệnh: `ipcs -m` (liệt kê), `ipcrm shm <id>` (cú pháp cũ) hoặc `ipcrm -m <id>`.
- Ưu/nhược: nhanh, nhiều process dùng chung được; nhưng phải tự đồng bộ, dùng chung một quy ước về key và bố cục dữ liệu, và dễ để lại segment mồ côi.

### Process semaphore, System V (5.2)
- Được cấp phát theo **tập** (set) gồm nhiều semaphore: `semget(key, nsems, flags)` → id. Xoá: `semctl(id, 0, IPC_RMID)` (phải tự xoá, không tự mất). Lệnh: `ipcs -s`, `ipcrm sem <id>`.
- Khởi tạo: `semctl(id, 0, SETALL, arg)` hoặc `SETVAL`; phải tự khai báo `union semun { int val; struct semid_ds *buf; unsigned short *array; };`. Mới tạo thì giá trị chưa xác định, nên luôn khởi tạo.
- Thao tác: `semop(id, ops, nops)` với mảng `struct sembuf { sem_num, sem_op, sem_flg }`. `sem_op` dương = post (cộng vào), âm = wait (chặn tới khi đủ để trừ mà không âm), 0 = chờ tới khi bằng 0. Mọi thao tác trong một lần semop là **nguyên tử**.
- `sem_flg`: `IPC_NOWAIT` (không chặn, trả về EAGAIN), **`SEM_UNDO`** (kernel tự hoàn tác khi process kết thúc, tránh semaphore bị kẹt khi process chết giữa vùng găng).
- (POSIX semaphore `sem_open`/`sem_wait` có API gọn hơn; xem `06-ipc`.)

### Mapped memory (5.3)
- `mmap` ánh xạ **file** vào bộ nhớ: đọc ghi file như một mảng, không cần read/write. Nhiều process map cùng file thì dùng chung dữ liệu → IPC (có file đi kèm, dữ liệu còn lại sau khi process thoát).
- `mmap(addr, length, prot, flags, fd, offset)`: addr thường NULL; offset là bội số page size. prot: `PROT_READ`, `PROT_WRITE`, `PROT_EXEC`. flags: `MAP_SHARED` (ghi được đưa xuống file và process khác thấy) **hoặc** `MAP_PRIVATE` (copy-on-write, ghi chỉ thấy trong process mình, không ghi xuống file), `MAP_FIXED` (bắt buộc dùng đúng addr, phải căn trang). Lỗi trả về `MAP_FAILED`. `munmap(addr, length)` để gỡ (exit cũng tự gỡ).
- File phải đủ dài trước khi map: sách dùng `lseek(fd, FILE_LENGTH + 1, SEEK_SET); write(fd, "", 1);` (nay hay dùng `ftruncate`). Truy cập phần vượt cuối file → **SIGBUS**.
- `msync(addr, length, flags)`: đẩy thay đổi xuống đĩa. `MS_ASYNC` (lên lịch ghi rồi return ngay), `MS_SYNC` (chờ ghi xong), `MS_INVALIDATE` (bỏ các bản map khác để đọc lại từ file). Linux có thể giữ thay đổi trong buffer, nên phải msync khi cần chắc dữ liệu đã nằm trên đĩa.
- Dùng chung qua mmap cũng cần đồng bộ (semaphore hoặc khoá file `fcntl`).
- Ứng dụng khác: map `/dev/zero` (hoặc `MAP_ANONYMOUS`) để có vùng nhớ khởi tạo bằng 0, chia sẻ được giữa cha và con sau fork; đọc file nhanh.

### Pipe (5.4)
- **Một chiều**: `int fds[2]; pipe(fds);` → `fds[0]` là **đầu đọc**, `fds[1]` là **đầu ghi**. Dữ liệu đi theo thứ tự FIFO. Chỉ dùng được giữa các process **có quan hệ họ hàng** (tạo pipe rồi fork; con kế thừa fd).
- Dung lượng có hạn: ghi khi pipe đầy thì chặn; đọc khi pipe rỗng (và đầu ghi còn mở) thì chặn. Mọi đầu ghi đã đóng → read trả về 0 (**EOF**). Ghi khi mọi đầu đọc đã đóng → **SIGPIPE** (hoặc EPIPE).
- Mỗi process phải **đóng đầu không dùng**. Nếu không, bên đọc không bao giờ thấy EOF (vì chính nó còn giữ một đầu ghi).
- `fdopen(fd, "r"/"w")` bọc fd thành `FILE*` để dùng fprintf/fgets (listing 5.7). Nhớ fflush hoặc fclose để dữ liệu thật sự đi vào pipe.
- **Chuyển hướng** (listing 5.8): trong con, `dup2(fds[0], STDIN_FILENO)` rồi exec `sort` → sort đọc từ pipe như stdin. `dup2(old, new)` đóng new (nếu đang mở) rồi cho new trỏ vào cùng file với old.
- **`popen(command, "r"/"w")`** (listing 5.9): tạo pipe + fork + exec `sh -c command`; "w" thì ta ghi vào stdin của lệnh, "r" thì đọc stdout của lệnh. **`pclose`** đóng pipe, chờ con và trả về wait status. Có cùng rủi ro bảo mật với system().
- Ghi tối đa **PIPE_BUF** byte (4 KB trên Linux) là **nguyên tử**: dữ liệu từ nhiều process ghi cùng lúc không bị trộn lẫn trong một lần ghi nhỏ như vậy.

### FIFO (5.4.5)
- **Named pipe**: pipe có tên trong filesystem (`ls -l` hiện ký tự `p`). Process **không có quan hệ họ hàng** cũng dùng được. Tạo bằng lệnh `mkfifo /tmp/fifo` hoặc hàm `mkfifo(path, mode)`.
- Mở như file thường: `open` để đọc thì chặn tới khi có process mở để ghi, và ngược lại. Nhiều process có thể cùng ghi; ghi ≤ PIPE_BUF byte thì nguyên tử.
- Khác Windows named pipe: FIFO trên Linux chỉ dùng trên một máy và là một chiều (không phải kiểu client-server).

### Socket (5.5)
- Kênh **hai chiều**, dùng được giữa các process trên cùng máy hoặc **qua mạng**.
- Tham số khi tạo `socket(namespace, style, protocol)`:
  - **Namespace**: `PF_LOCAL`/`PF_UNIX` (cùng máy, địa chỉ là đường dẫn file), `PF_INET` (IPv4: địa chỉ IP + port).
  - **Style**: `SOCK_STREAM` (như cuộc gọi điện thoại: có kết nối, dữ liệu đúng thứ tự, không mất, như TCP), `SOCK_DGRAM` (như gửi thư: từng gói riêng, có thể mất hoặc đảo thứ tự, như UDP).
  - **Protocol**: thường là 0 (giao thức mặc định).
- System call: `socket` (tạo), `close` (huỷ), `connect` (client nối tới server), `bind` (gán địa chỉ cho socket server), `listen` (cho phép nhận kết nối, kèm độ dài hàng đợi), `accept` (nhận một kết nối, trả về **socket mới** cho kết nối đó; socket lắng nghe vẫn tiếp tục nhận client khác).
- Server: socket → bind → listen → accept (lặp) → read/write → close. Client: socket → connect → write/read → close. Dùng `read`/`write` (hoặc `send`/`recv`) như với file.
- **Local socket** (listing 5.10, 5.11): `struct sockaddr_un`, `sun_family = AF_LOCAL`, `sun_path` = đường dẫn (tối đa 108 byte). File socket phải được `unlink` khi xong. Process cần quyền ghi vào thư mục để tạo socket.
- **Internet socket** (listing 5.12): `struct sockaddr_in`, `sin_family = AF_INET`, `sin_port = htons(80)`, `sin_addr`. Phân giải tên máy bằng `gethostbyname` (nay dùng `getaddrinfo`). **Byte order**: số trên mạng là big-endian ("network byte order") → `htons`/`htonl` (host to network), `ntohs`/`ntohl`. Port < 1024 chỉ root mới bind được. Dịch vụ chuẩn có port cố định (`/etc/services`: HTTP 80...).
- **`socketpair(PF_LOCAL, SOCK_STREAM, 0, fds)`**: tạo một cặp socket đã nối sẵn, giống pipe nhưng **hai chiều** (mỗi fd vừa đọc vừa ghi). Chỉ dùng với local namespace và giữa các process có quan hệ họ hàng.

## Trắc nghiệm

### Cơ chế IPC nào nhanh nhất theo sách?
- [ ] Pipe
- [ ] Socket
- [x] Shared memory
- [ ] FIFO
> Sau khi attach, đọc ghi là truy cập bộ nhớ bình thường, không có system call hay copy qua kernel. Cái giá là phải tự đồng bộ.

### Gọi `shmget(IPC_PRIVATE, size, IPC_CREAT | 0600)` thì?
- [ ] Lấy segment chung của cả hệ thống
- [x] Luôn tạo một segment mới; các process khác chỉ dùng được nếu biết id (ví dụ con sau fork)
- [ ] Segment chỉ dùng được bởi thread
- [ ] Lỗi vì thiếu key
> IPC_PRIVATE bảo đảm không đụng segment có sẵn. Process không quan hệ muốn dùng chung thì phải thống nhất một key (thường tạo bằng `ftok`).

### Tất cả các process dùng một shared memory segment đã thoát mà không gọi IPC_RMID. Segment thế nào?
- [ ] Tự bị xoá khi process cuối thoát
- [x] Vẫn tồn tại trong hệ thống (xem bằng `ipcs -m`) tới khi bị xoá bằng IPC_RMID/ipcrm hoặc reboot
- [ ] Bị ghi xuống đĩa
- [ ] Trở thành file trong /tmp
> Đây là rò rỉ tài nguyên hay gặp với System V IPC. Semaphore set cũng vậy.

### Hàm nào gắn một shared memory segment vào không gian địa chỉ của process?
- [ ] `shmget`
- [x] `shmat`
- [ ] `shmctl`
- [ ] `mmap`
> shmget tạo/lấy id; shmat gắn và trả về địa chỉ; shmdt gỡ; shmctl lấy thông tin hoặc xoá.

### `shm_nattch` trong `struct shmid_ds` cho biết gì?
- [ ] Kích thước segment
- [x] Số lần segment đang được attach
- [ ] PID của người tạo
- [ ] Số semaphore liên quan
> Sau IPC_RMID, segment chỉ thật sự bị huỷ khi nattch về 0.

### Kích thước shared memory segment nên là gì?
- [ ] Số nguyên tố
- [x] Bội số của page size (getpagesize(), thường 4096)
- [ ] Đúng bằng sizeof dữ liệu
- [ ] Lũy thừa của 10
> Kernel cấp phát theo trang; xin 100 byte thì thực tế vẫn chiếm một trang.

### Cờ `SEM_UNDO` trong semop dùng để làm gì?
- [ ] Hoàn tác thao tác semop vừa thực hiện ngay lập tức
- [x] Kernel tự hoàn tác thay đổi của process khi nó kết thúc, để semaphore không bị kẹt nếu process chết giữa vùng găng
- [ ] Cho phép giá trị âm
- [ ] Không chặn khi không đủ giá trị
> Không chặn là IPC_NOWAIT.

### Trong semop, `sem_op = -1` có ý nghĩa gì?
- [ ] Xoá semaphore
- [x] Wait (P): trừ 1, chặn nếu giá trị hiện tại bằng 0
- [ ] Post (V): cộng 1
- [ ] Chờ giá trị bằng 0
> sem_op dương = post; sem_op = 0 = chờ tới khi semaphore bằng 0.

### Khác nhau chính giữa MAP_SHARED và MAP_PRIVATE?
- [ ] MAP_SHARED chỉ đọc, MAP_PRIVATE đọc ghi
- [x] MAP_SHARED ghi xuống file và process khác map cùng file thấy được; MAP_PRIVATE là copy-on-write, ghi chỉ thấy trong process đó
- [ ] MAP_PRIVATE nhanh hơn vì không có page cache
- [ ] MAP_SHARED chỉ dùng với /dev/zero
> Hai cờ loại trừ nhau, phải chọn đúng một. IPC qua mmap dùng MAP_SHARED.

### mmap thất bại trả về gì?
- [ ] NULL
- [ ] -1 kiểu int
- [x] `MAP_FAILED` (tức `(void *)-1`)
- [ ] 0
> Kiểm tra `if (p == MAP_FAILED)`, không kiểm tra NULL.

### Vì sao trước khi mmap một file mới tạo, sách gọi `lseek` tới cuối rồi `write` một byte?
- [ ] Để mở file ở chế độ ghi
- [x] Để file có đủ kích thước; truy cập vùng map vượt quá cuối file gây SIGBUS
- [ ] Để xoá nội dung cũ
- [ ] Vì mmap chỉ hoạt động với file có dữ liệu
> Cách hiện đại: `ftruncate(fd, length)`.

### `msync(addr, len, MS_SYNC)` làm gì?
- [ ] Đồng bộ hai process
- [x] Ghi các thay đổi trong vùng map xuống file trên đĩa và chờ ghi xong
- [ ] Khoá vùng nhớ
- [ ] Gỡ ánh xạ
> MS_ASYNC chỉ lên lịch ghi rồi return ngay; MS_INVALIDATE làm các bản map khác đọc lại dữ liệu mới.

### Sau `pipe(fds)`, đầu nào dùng để đọc?
- [x] `fds[0]`
- [ ] `fds[1]`
- [ ] Cả hai
- [ ] Tuỳ thuộc ai gọi read trước
> Mẹo nhớ: 0 giống stdin (đọc), 1 giống stdout (ghi).

### Process con đọc pipe nhưng không bao giờ nhận được EOF dù cha đã đóng đầu ghi. Nguyên nhân thường gặp?
- [ ] Pipe bị đầy
- [x] Chính process con (hoặc một process khác) vẫn còn giữ fd đầu ghi chưa đóng
- [ ] Cha phải gửi ký tự '\0'
- [ ] Phải dùng FIFO
> EOF chỉ xuất hiện khi mọi fd trỏ tới đầu ghi đều đã đóng. Sau fork, mỗi process phải đóng đầu mình không dùng.

### Ghi vào pipe mà mọi đầu đọc đều đã đóng thì sao?
- [ ] Ghi thành công, dữ liệu bị bỏ
- [ ] Chặn mãi mãi
- [x] Process nhận SIGPIPE (mặc định bị giết); nếu bỏ qua SIGPIPE thì write trả về -1 với errno = EPIPE
- [ ] Trả về 0
> Ví dụ `yes | head -1`: head thoát, yes nhận SIGPIPE và kết thúc.

### Hàm nào bọc một file descriptor thành `FILE *` để dùng fprintf/fgets?
- [ ] `fopen`
- [x] `fdopen`
- [ ] `fileno`
- [ ] `freopen`
> fileno làm ngược lại: lấy fd từ FILE*.

### Để chương trình `sort` (chạy bằng exec trong con) đọc dữ liệu từ pipe, con cần làm gì trước exec?
- [ ] `dup2(STDIN_FILENO, fds[0])`
- [x] `dup2(fds[0], STDIN_FILENO)`, đóng các fd pipe thừa, rồi exec
- [ ] `close(STDIN_FILENO)` là đủ
- [ ] Truyền fds[0] làm đối số dòng lệnh
> `dup2(old, new)` làm new trỏ vào cùng file với old. Fd được giữ nguyên qua exec, nên sort đọc fd 0 là đọc từ pipe.

### `popen("sort", "w")` trả về gì?
- [ ] PID của sort
- [x] `FILE *` để ghi vào stdin của sort
- [ ] `FILE *` để đọc output của sort
- [ ] fd của pipe
> Đóng bằng `pclose`, không dùng fclose. pclose chờ lệnh kết thúc và trả về wait status của nó.

### Điều nào ĐÚNG về FIFO (named pipe)? (chọn tất cả đáp án đúng)
- [x] Có tên trong filesystem, tạo bằng mkfifo
- [x] Process không có quan hệ họ hàng dùng được
- [x] open để đọc chặn tới khi có process mở để ghi
- [ ] Dữ liệu được lưu trên đĩa sau khi process thoát
> Dữ liệu đi qua kernel, không lưu trên đĩa; file FIFO chỉ là một điểm hẹn có tên.

### Ghi vào pipe/FIFO bao nhiêu byte thì được đảm bảo nguyên tử (không bị trộn với dữ liệu của process khác)?
- [ ] Bất kỳ kích thước nào
- [x] Tối đa PIPE_BUF byte (4 KB trên Linux)
- [ ] Tối đa 1 byte
- [ ] Tối đa 64 KB
> Lần ghi lớn hơn PIPE_BUF có thể bị chia nhỏ và xen kẽ với dữ liệu của người ghi khác.

### Khác nhau giữa SOCK_STREAM và SOCK_DGRAM?
- [x] STREAM có kết nối, dữ liệu đúng thứ tự và không mất (như TCP); DGRAM gửi từng gói độc lập, có thể mất hoặc đảo thứ tự (như UDP)
- [ ] STREAM chỉ dùng cho file, DGRAM cho mạng
- [ ] STREAM một chiều, DGRAM hai chiều
- [ ] Không khác, chỉ khác tên
> Sách ví SOCK_STREAM như cuộc gọi điện thoại, SOCK_DGRAM như gửi thư.

### Thứ tự gọi đúng ở phía server với SOCK_STREAM?
- [ ] socket → connect → accept
- [ ] socket → listen → bind → accept
- [x] socket → bind → listen → accept
- [ ] bind → socket → accept → listen
> Client: socket → connect. accept trả về socket mới cho từng kết nối.

### `accept` trả về gì?
- [ ] 0 khi thành công
- [x] Một socket mới dành riêng cho kết nối vừa nhận; socket lắng nghe vẫn tiếp tục dùng được
- [ ] Địa chỉ IP của client
- [ ] Socket lắng nghe ban đầu
> Server thường lặp accept, rồi xử lý từng kết nối (có thể fork hoặc tạo thread cho mỗi kết nối).

### Vì sao phải gọi `htons(port)` khi gán `sin_port`?
- [ ] Để kiểm tra port hợp lệ
- [x] Để đổi số từ thứ tự byte của máy (x86: little-endian) sang network byte order (big-endian)
- [ ] Để mã hoá port
- [ ] Vì sin_port là chuỗi
> htons/htonl (host to network short/long), ntohs/ntohl (chiều ngược lại). Trên máy big-endian các hàm này không làm gì cả.

### Địa chỉ của local socket (PF_LOCAL) là gì?
- [ ] IP 127.0.0.1 và một port
- [x] Một đường dẫn trong filesystem (sun_path), cần unlink sau khi dùng xong
- [ ] PID của server
- [ ] Tên máy
> Local socket chỉ dùng trên cùng một máy và nhanh hơn socket qua loopback.

### Port nào chỉ process root mới bind được?
- [x] Dưới 1024
- [ ] Trên 49152
- [ ] Port 0
- [ ] Mọi port đều cần root
> Port 0 trong bind nghĩa là để kernel tự chọn một port trống.

### Điều nào ĐÚNG về socketpair? (chọn tất cả đáp án đúng)
- [x] Tạo hai socket đã được nối với nhau
- [x] Giao tiếp hai chiều: mỗi đầu vừa đọc vừa ghi
- [x] Chỉ dùng với local namespace (PF_LOCAL)
- [ ] Dùng được giữa hai máy qua mạng
> Giống một pipe hai chiều; thường tạo trước fork để cha và con nói chuyện với nhau.

### Cơ chế nào yêu cầu các process phải có quan hệ họ hàng (cha/con, cùng tổ tiên)?
- [x] Pipe (không tên)
- [ ] FIFO
- [ ] Shared memory dùng key chung
- [ ] Socket Internet
> Pipe chỉ tồn tại dưới dạng fd, nên chỉ truyền được qua fork. FIFO, shared memory với key, và socket có tên/địa chỉ để process bất kỳ tìm đến.
