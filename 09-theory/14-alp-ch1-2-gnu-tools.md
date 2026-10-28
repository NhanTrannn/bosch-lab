# ALP ch.1–2: Công cụ GNU và viết phần mềm Linux tốt

## Lý thuyết

Theo *Advanced Linux Programming* (Mitchell, Oldham, Samuel), chương 1 "Getting Started" và chương 2 "Writing Good GNU/Linux Software". Lab: `10-alp/01` … `10-alp/06`. Xem thêm chủ đề "Build, link và debug".

### Biên dịch bằng GCC (1.2)
- `gcc` cho C, `g++` cho C++. Biên dịch một file thành object: `gcc -c main.c` → `main.o`. Link: `g++ -o reciprocal main.o reciprocal.o` (có code C++ thì link bằng g++ để kéo libstdc++).
- Tùy chọn hay dùng: `-I dir` (thêm thư mục tìm header), `-D NAME=value` (định nghĩa macro, ví dụ `-DNDEBUG` tắt assert), `-O2` (tối ưu), `-g` (thông tin debug), `-o file` (tên output), `-Wall` (bật cảnh báo), `-L dir` và `-l name` (thư mục và tên thư viện: `-lm` → `libm.so`/`libm.a`), `-static`, `-shared`, `-fPIC`, `-pg` (gprof).
- Tối ưu (`-O2`) làm debug khó hơn (biến bị bỏ đi, dòng code bị sắp xếp lại); khi debug dùng `-g` và không tối ưu.

### GNU Make (1.3)
- Makefile gồm các **rule**: `target: dependencies` rồi dòng lệnh bắt đầu bằng **TAB**. Make chỉ build lại target khi một dependency **mới hơn** target (so thời gian sửa đổi).
- Target mặc định là target đầu tiên. Target giả như `clean` không tạo ra file (nên khai báo `.PHONY`).
- Biến: `CFLAGS = -O2`; ghi đè từ dòng lệnh: `make CFLAGS=-g`. Make có sẵn rule ngầm (implicit rule) để build `.o` từ `.c`.
- `make -j4`: chạy song song; `make -n`: chỉ in lệnh, không chạy.

### GDB (1.4)
- Biên dịch với `-g`. `gdb ./prog` → `run args` → khi crash: `where`/`backtrace` (xem call stack), `up`/`down` (chuyển frame), `print expr`.
- `break main` / `break file.c:20`, `next` (chạy dòng tiếp, bước qua hàm), `step` (đi vào hàm), `continue`, `finish`, `info locals`, `quit`.
- Sách lưu ý: chương trình crash nhưng không có `-g` thì backtrace chỉ có địa chỉ, không có số dòng.

### Tìm thông tin (1.5)
- **man**: section 1 (lệnh người dùng), **2 (system call)**, **3 (hàm thư viện C)**, 8 (lệnh quản trị). `man 2 open`, `man 3 printf`, `man sleep` vs `man 3 sleep`. `man -k keyword` (= `apropos`), `whatis`.
- **info**: tài liệu GNU dạng siêu văn bản (gcc, libc, make, emacs).
- Header: `/usr/include`, `/usr/include/sys`, `/usr/include/bits` (định nghĩa phụ thuộc kiến trúc), `/usr/include/linux`. Source của Linux và glibc là tài liệu chính xác nhất.

### Tương tác với môi trường thực thi (2.1)
- `int main(int argc, char *argv[])`: `argc` = số phần tử, `argv[0]` = tên chương trình, `argv[argc]` = NULL.
- **Quy ước dòng lệnh GNU**: tùy chọn ngắn `-h` (gộp được `-vo`), tùy chọn dài `--help`, `--output=file`. Nên có `--help` và `--version`. `--` kết thúc phần tùy chọn.
- **getopt_long** (`<getopt.h>`): chuỗi short options `"ho:v"` (`:` = cần đối số), mảng `struct option {const char *name; int has_arg; int *flag; int val;}` kết thúc bằng `{NULL, 0, NULL, 0}`. Gọi lặp tới khi trả về -1; `optarg` = đối số của tùy chọn; `optind` = chỉ số của đối số thường đầu tiên; `'?'` = tùy chọn sai (lỗi in ra stderr). GNU getopt hoán vị argv để đối số thường nằm cuối.
- **stdin/stdout/stderr** = fd 0/1/2. Thông báo lỗi ghi ra stderr để tách khỏi output. **stdout có buffer** (line-buffered với terminal, full-buffered với file/pipe), **stderr không buffer**. `fflush(stdout)` để đẩy ngay. C++: `cout` có buffer, `cerr` không; `endl` flush, `'\n'` không.
- Gộp stderr vào stdout: `prog > out.txt 2>&1` (2>&1 đứng sau redirect file), `prog 2>&1 | filter` (đứng trước pipe).
- **Exit code**: 0 = thành công, khác 0 = lỗi; shell lưu trong `$?`. Chỉ 8 bit thấp được giữ → dùng 0–127; bị signal giết thì shell báo **128 + số hiệu signal**.
- **Môi trường**: các cặp `TEN=giatri` (tên viết hoa theo quy ước): `USER`, `HOME`, `PATH`, `DISPLAY`. Shell: `printenv`, `echo $HOME`, `export EDITOR=emacs`. C: `getenv` (NULL nếu không có), `setenv`, `unsetenv`, duyệt `extern char **environ;` (không sửa trực tiếp). Process con nhận **bản copy** môi trường của cha. Hay dùng để cấu hình (ví dụ `SERVER_NAME`, có giá trị mặc định khi biến không tồn tại).
- **File tạm** (thường trong `/tmp`): phải tránh trùng tên giữa nhiều instance, quyền chỉ cho chủ sở hữu, tên không đoán trước được. `mkstemp("/tmp/x.XXXXXX")` → fd, quyền 0600, tạo bằng O_EXCL; nên `unlink` ngay nếu chỉ dùng nội bộ (file tự mất khi đóng fd, kể cả khi crash). `tmpfile()` → `FILE*` đã unlink. **Không** dùng `mktemp`, `tmpnam`, `tempnam`.

### Lập trình phòng thủ (2.2)
- **assert(expr)** (`<assert.h>`): sai thì in file, dòng, biểu thức rồi `abort()`. `-DNDEBUG` xoá mọi assert → biểu thức trong assert **không được có tác dụng phụ** (gọi hàm, gán, `++`). Dùng cho kiểm tra nội bộ (con trỏ NULL, điều kiện tham số), không dùng để kiểm tra input của người dùng.
- **System call có thể lỗi** vì: hết tài nguyên (bộ nhớ, giới hạn số file mở), không có quyền, tham số sai, lỗi bên ngoài (đĩa hỏng, mạng), hoặc bị **signal ngắt** giữa chừng.
- Lỗi: thường trả về -1 (hàm trả con trỏ thì NULL) và đặt **errno** (`<errno.h>`, giá trị như `ENOENT`, `EACCES`, `EINVAL`, `EINTR`, `ENOMEM`). errno chỉ có nghĩa ngay sau lời gọi thất bại; lời gọi thành công không xoá nó → lưu errno vào biến trước khi gọi hàm khác. Mỗi thread có errno riêng.
- `strerror(errno)` → chuỗi mô tả; `perror("msg")` in `msg: mô tả` ra stderr.
- **EINTR**: system call chặn (read, sleep, wait...) bị signal ngắt → gọi lại.
- Sách phân loại cách xử lý: lỗi do bug (dùng assert/abort), lỗi do môi trường hay input (báo lỗi rõ ràng, thoát với exit code khác 0 hoặc khôi phục). Khi lỗi giữa chừng phải **giải phóng tài nguyên đã cấp** (bộ nhớ, fd) trước khi return.

### Thư viện (2.3)
- **Archive (static library)** `.a`: tập hợp các `.o`. Tạo: `ar cr libtest.a test1.o test2.o`. Link: `gcc -o app app.o -L. -ltest`. Linker chỉ lấy các object trong archive cần cho **symbol chưa giải quyết tính tới lúc đó** → thư viện phải đứng **sau** file dùng nó trên dòng lệnh (`gcc -L. -ltest app.o` sẽ lỗi undefined reference).
- **Shared library** `.so`: biên dịch với `-fPIC` (position-independent code: nạp ở địa chỉ nào cũng chạy được), tạo bằng `gcc -shared -fPIC -o libtest.so test1.o test2.o`. Code của thư viện không bị copy vào file chạy; được nạp lúc chạy và **dùng chung** trong bộ nhớ giữa các process.
- Có cả `libtest.a` và `libtest.so` → linker **ưu tiên .so**, trừ khi dùng `-static`.
- Lúc chạy, dynamic loader tìm `.so` trong `/lib`, `/usr/lib`, các thư mục trong `/etc/ld.so.conf` (chạy `ldconfig`), `LD_LIBRARY_PATH` (khi phát triển), hoặc đường dẫn ghi sẵn bằng `-Wl,-rpath,/opt/lib`. `ldd app` liệt kê thư viện động mà app cần.
- Thư viện chuẩn: `libc` (tự link), `libm` (toán, phải `-lm`), `libstdc++` (g++ tự link), `libpthread` (`-lpthread`).
- **Phụ thuộc**: shared library tự ghi nó cần thư viện nào; archive thì không → khi link `-static` phải tự liệt kê đủ (ví dụ `-ltiff -ljpeg -lz`).
- **Ưu nhược điểm**: shared tiết kiệm đĩa/RAM và cập nhật thư viện không cần link lại, nhưng phải cài đúng phiên bản ở máy chạy và một bản cập nhật lỗi có thể làm hỏng mọi chương trình dùng nó. Static thì độc lập, file lớn hơn; sách khuyên dùng static cho chương trình setuid hoặc khi phân phối mà không kiểm soát được máy đích.
- **Nạp động** (`<dlfcn.h>`, link `-ldl`): `dlopen("libtest.so", RTLD_LAZY)` → handle, `dlsym(handle, "my_function")` → địa chỉ, `dlclose(handle)`, `dlerror()` → chuỗi lỗi. Dùng cho plugin. Hàm C++ cần `extern "C"` để dlsym tìm được tên (tránh name mangling).

## Trắc nghiệm

### Lệnh nào chỉ biên dịch `main.c` thành file object mà không link?
- [ ] `gcc main.c`
- [x] `gcc -c main.c`
- [ ] `gcc -o main.c`
- [ ] `gcc -S main.c`
> `-c` dừng sau bước biên dịch, tạo `main.o`. `-S` dừng ở mã assembly (`main.s`). Không có tùy chọn thì gcc biên dịch và link luôn thành `a.out`.

### Chương trình gồm `main.c` và `reciprocal.cpp`. Nên link bằng lệnh nào?
- [ ] `gcc -o reciprocal main.o reciprocal.o`
- [x] `g++ -o reciprocal main.o reciprocal.o`
- [ ] `ld -o reciprocal main.o reciprocal.o`
- [ ] `ar cr reciprocal main.o reciprocal.o`
> Có code C++ thì link bằng g++ để tự kéo thư viện chuẩn C++ (libstdc++). Dùng gcc thường bị undefined reference tới các symbol của std.

### Tùy chọn `-D NDEBUG` khi biên dịch có tác dụng gì?
- [ ] Bật chế độ debug
- [x] Định nghĩa macro NDEBUG, làm mọi `assert` bị xoá khỏi chương trình
- [ ] Tắt mọi cảnh báo
- [ ] Không sinh thông tin debug
> `-D` định nghĩa macro như `#define`. assert.h kiểm tra NDEBUG để quyết định có sinh code kiểm tra hay không.

### Dòng lệnh trong một rule của Makefile phải bắt đầu bằng gì?
- [ ] 4 dấu cách
- [x] Một ký tự TAB
- [ ] Dấu `$`
- [ ] Dấu `@`
> Thiếu TAB thì make báo "missing separator". `@` đứng đầu lệnh (sau TAB) chỉ để không in lệnh ra trước khi chạy.

### Make quyết định build lại một target khi nào?
- [ ] Mỗi lần chạy make
- [x] Khi target chưa tồn tại hoặc có dependency mới hơn target (theo thời gian sửa đổi)
- [ ] Khi nội dung file nguồn thay đổi (so sánh hash)
- [ ] Chỉ khi chạy `make clean` trước
> Make so sánh timestamp, không so nội dung. `touch file.c` đủ để buộc build lại.

### Trong gdb, chương trình vừa crash vì segfault. Lệnh nào xem chuỗi lời gọi hàm dẫn tới chỗ crash?
- [ ] `print`
- [ ] `next`
- [x] `where` (hoặc `backtrace`)
- [ ] `list`
> Sau đó dùng `up`/`down` để chuyển giữa các frame và `print` để xem biến ở frame đó. Cần biên dịch với `-g` mới có tên hàm và số dòng.

### Trong gdb, khác nhau giữa `next` và `step`?
- [ ] Không khác
- [x] `next` chạy hết lời gọi hàm trên dòng hiện tại như một bước; `step` đi vào bên trong hàm
- [ ] `step` chạy tới breakpoint tiếp theo
- [ ] `next` chạy ngược lại một dòng
> `finish` chạy tới khi hàm hiện tại return; `continue` chạy tới breakpoint tiếp theo.

### Muốn xem trang man của system call `open` (không phải lệnh shell), gõ gì?
- [ ] `man open`
- [x] `man 2 open`
- [ ] `man 3 open`
- [ ] `info open`
> Section 1: lệnh người dùng, 2: system call, 3: hàm thư viện C, 8: lệnh quản trị. Ví dụ `man 3 sleep` là hàm C, còn `man sleep` là lệnh shell.

### Với `prog -o out.txt in1`, `argc` và `argv[0]` là gì?
- [ ] 3 và "-o"
- [x] 4 và "prog" (tên chương trình như khi được gọi)
- [ ] 3 và "prog"
- [ ] 4 và "out.txt"
> argv gồm cả tên chương trình: {"prog", "-o", "out.txt", "in1"}, và `argv[argc]` là NULL.

### Trong chuỗi short options `"ho:v"` của getopt_long, dấu `:` sau `o` nghĩa là?
- [ ] `-o` là tùy chọn bắt buộc
- [x] `-o` cần một đối số, lấy qua biến `optarg`
- [ ] `-o` có thể lặp lại nhiều lần
- [ ] `-o` là tùy chọn dài
> Ví dụ `-o out.txt` hoặc `-oout.txt`. Hai dấu `::` nghĩa là đối số tùy chọn (GNU extension).

### Sau vòng lặp getopt_long (trả về -1), `optind` cho biết gì?
- [ ] Số tùy chọn đã xử lý
- [x] Chỉ số trong argv của đối số không phải tùy chọn đầu tiên
- [ ] Chỉ số của tùy chọn bị lỗi
- [ ] Luôn bằng argc
> Duyệt các file đầu vào bằng `for (i = optind; i < argc; ++i)`. GNU getopt đã hoán vị argv để các đối số thường nằm cuối.

### getopt_long trả về gì khi gặp tùy chọn không được khai báo?
- [ ] -1
- [ ] 0
- [x] `'?'`
- [ ] Ký tự của tùy chọn đó
> Ký tự gây lỗi nằm trong `optopt`; getopt in thông báo ra stderr (trừ khi `opterr = 0`). Listing 2.2 khi đó in hướng dẫn ra stderr và thoát với exit code 1.

### Vì sao vòng lặp sau không in mỗi giây một dấu chấm ra terminal?
```c
while (1) { printf("."); sleep(1); }
```
- [ ] printf bị lỗi khi không có `\n`
- [x] stdout có buffer, chưa có `\n` nên dấu chấm nằm trong buffer
- [ ] sleep chặn cả printf
- [ ] Terminal không hiển thị dấu chấm
> Dùng `fprintf(stderr, ".")` (stderr không buffer) hoặc gọi `fflush(stdout)` sau mỗi printf.

### Lệnh nào gộp cả stdout và stderr của `prog` vào file `out.txt`?
- [ ] `prog 2>&1 > out.txt`
- [x] `prog > out.txt 2>&1`
- [ ] `prog > out.txt 1>&2`
- [ ] `prog | out.txt 2>&1`
> Redirect được xử lý từ trái sang phải: `> out.txt` trỏ fd 1 vào file rồi `2>&1` cho fd 2 trỏ theo. Viết ngược thì fd 2 vẫn ra terminal. Với pipe: `prog 2>&1 | filter`.

### Chương trình gọi `exit(256)`. `echo $?` in ra?
- [ ] 256
- [x] 0
- [ ] 1
- [ ] 255
> Chỉ 8 bit thấp được giữ: 256 mod 256 = 0, tức là shell hiểu là "thành công". Sách khuyên chỉ dùng exit code 0–127.

### Chương trình bị `kill -TERM` (SIGTERM = 15). `echo $?` trong bash in ra?
- [ ] 15
- [ ] 0
- [x] 143
- [ ] 255
> Bị signal giết thì shell báo 128 + số hiệu signal. Ctrl+C (SIGINT = 2) cho 130.

### Process cha gọi `setenv("MODE", "debug", 1)` rồi fork. Điều nào ĐÚNG? (chọn tất cả đáp án đúng)
- [x] Process con thấy MODE=debug
- [x] Chương trình con exec (execvp) cũng thấy MODE=debug
- [ ] Shell đã chạy process cha cũng thấy MODE=debug
- [ ] Nếu con gọi unsetenv("MODE") thì cha cũng mất biến
> Môi trường được copy từ cha sang con lúc fork và giữ qua exec. Không process nào đổi được môi trường của process cha hay của process khác.

### `getenv("SERVER_NAME")` khi biến không tồn tại trả về gì?
- [ ] Chuỗi rỗng ""
- [x] NULL
- [ ] Ném lỗi, errno = ENOENT
- [ ] "SERVER_NAME"
> Listing 2.4 kiểm tra NULL để dùng giá trị mặc định.

### Vì sao sách khuyên dùng `mkstemp` thay vì `tmpnam` + `open`?
- [ ] mkstemp nhanh hơn
- [x] tmpnam chỉ sinh tên; giữa lúc sinh tên và lúc mở, kẻ tấn công có thể tạo trước file đó (race condition). mkstemp tạo và mở file trong một bước với quyền 0600
- [ ] tmpnam không còn trong glibc
- [ ] mkstemp tự xoá file khi chương trình kết thúc
> mkstemp không tự xoá file; phải unlink. tmpfile thì trả về file đã được unlink sẵn.

### Gọi `unlink` lên file tạm ngay sau khi mkstemp mở nó. Điều gì xảy ra?
- [ ] Không đọc ghi qua fd được nữa
- [x] Tên file biến mất nhưng dữ liệu còn đọc ghi được qua fd; file thật sự bị xoá khi fd đóng (kể cả khi chương trình crash)
- [ ] unlink trả về lỗi EBUSY vì file đang mở
- [ ] File bị xoá ngay và fd trỏ vào rác
> File được đếm tham chiếu: chỉ bị huỷ khi không còn tên (link) và không còn fd nào mở.

### Đoạn code có vấn đề gì?
```c
assert(do_something() == 0);
```
- [ ] assert không nhận biểu thức so sánh
- [x] Khi biên dịch với -DNDEBUG, cả lời gọi do_something() bị xoá
- [ ] assert chỉ dùng trong C++
- [ ] Không có vấn đề
> Viết `int status = do_something(); assert(status == 0);`. Biểu thức trong assert không được có tác dụng phụ.

### Nên dùng assert cho trường hợp nào?
- [ ] Kiểm tra người dùng nhập số hợp lệ
- [ ] Kiểm tra file cấu hình tồn tại
- [x] Kiểm tra con trỏ tham số không phải NULL theo đúng hợp đồng của hàm
- [ ] Kiểm tra kết nối mạng thành công
> assert dành cho lỗi lập trình (điều "không bao giờ được xảy ra"). Lỗi input/môi trường phải được xử lý và báo lỗi tử tế, vì assert có thể bị tắt và làm crash chương trình.

### Sau lời gọi thất bại `open()` (errno = ENOENT), chương trình gọi tiếp `getpid()` (thành công). errno lúc này là?
- [ ] 0
- [x] Vẫn là ENOENT
- [ ] Không xác định
- [ ] EINVAL
> Lời gọi thành công không xoá errno. Chỉ đọc errno ngay sau khi hàm báo lỗi, và lưu lại nếu còn gọi hàm khác (kể cả printf).

### Một system call chặn (read, wait) trả về -1 với errno = EINTR. Nghĩa là gì và nên làm gì?
- [ ] File bị hỏng, phải thoát
- [x] Lời gọi bị một signal ngắt giữa chừng; thường chỉ cần gọi lại
- [ ] Không đủ quyền
- [ ] Tham số sai
> Mẫu: `do { n = read(fd, buf, len); } while (n == -1 && errno == EINTR);`. Cờ `SA_RESTART` của sigaction giúp nhiều system call tự khởi động lại.

### errno có an toàn khi dùng trong chương trình nhiều thread không?
- [ ] Không, mọi thread dùng chung một errno
- [x] Có, mỗi thread có bản errno riêng
- [ ] Chỉ khi khoá mutex
- [ ] Chỉ trong thread chính
> Trong glibc, errno là macro trỏ tới biến thread-local. Ngược lại, các hàm pthread trả về mã lỗi trực tiếp, không đặt errno.

### Lệnh nào tạo thư viện tĩnh `libtest.a` từ `test1.o` và `test2.o`?
- [ ] `gcc -shared -o libtest.a test1.o test2.o`
- [x] `ar cr libtest.a test1.o test2.o`
- [ ] `ld -o libtest.a test1.o test2.o`
- [ ] `gcc -static -o libtest.a test1.o test2.o`
> `c`: tạo archive nếu chưa có, `r`: thêm hoặc thay các object. Thư viện động tạo bằng `gcc -shared -fPIC`.

### Lệnh `gcc -o app -L. -ltest app.o` báo undefined reference tới hàm trong libtest.a, dù hàm có trong thư viện. Vì sao?
- [ ] Thiếu `-fPIC`
- [x] Linker xử lý từ trái sang phải; lúc đọc libtest.a chưa có symbol nào cần, nên không lấy object nào. Phải đặt `-ltest` sau `app.o`
- [ ] Archive không dùng được với -L
- [ ] Phải dùng `-static`
> Với archive, linker chỉ lấy các object giải quyết được symbol đang còn thiếu. Thư viện phụ thuộc lẫn nhau có thể phải ghi lại nhiều lần.

### `-fPIC` dùng để làm gì khi tạo thư viện động?
- [ ] Bật tối ưu
- [x] Sinh mã độc lập vị trí, để thư viện nạp ở địa chỉ nào cũng chạy được và dùng chung giữa các process
- [ ] Gắn thông tin debug
- [ ] Biên dịch tĩnh
> PIC = Position-Independent Code: truy cập dữ liệu/hàm toàn cục qua bảng (GOT/PLT) thay vì địa chỉ tuyệt đối.

### Thư mục có cả `libtest.a` và `libtest.so`. `gcc -o app app.o -L. -ltest` dùng bản nào?
- [ ] libtest.a
- [x] libtest.so, trừ khi thêm `-static`
- [ ] Báo lỗi ambiguous
- [ ] Bản nào mới hơn
> Linker ưu tiên shared library. `-static` buộc dùng archive (và link tĩnh cả libc).

### Chương trình link với `libtest.so` nằm ở `/home/me/lib`. Chạy thì báo "cannot open shared object file". Cách nào giải quyết được? (chọn tất cả đáp án đúng)
- [x] `export LD_LIBRARY_PATH=/home/me/lib` rồi chạy lại
- [x] Link lại với `-Wl,-rpath,/home/me/lib`
- [x] Thêm thư mục vào `/etc/ld.so.conf` rồi chạy `ldconfig` (cần root)
- [ ] Thêm `-I/home/me/lib` khi biên dịch
> `-L` chỉ giúp lúc link; lúc chạy dynamic loader có danh sách tìm kiếm riêng. `-I` là thư mục header. `ldd app` cho thấy thư viện nào chưa tìm thấy ("not found").

### Hàm `sqrt` biên dịch được nhưng link báo `undefined reference to 'sqrt'`. Thiếu gì?
- [ ] `#include <math.h>`
- [x] `-lm` khi link
- [ ] `-fPIC`
- [ ] `-DMATH`
> Hàm toán nằm trong libm, không tự link như libc. Thiếu `#include` thì là lỗi/cảnh báo lúc biên dịch, không phải lỗi link.

### Ưu điểm của thư viện động so với thư viện tĩnh? (chọn tất cả đáp án đúng)
- [x] File thực thi nhỏ hơn
- [x] Nhiều process dùng chung một bản trong bộ nhớ
- [x] Sửa lỗi thư viện không cần link lại các chương trình
- [ ] Chương trình chạy được trên máy không cài thư viện
> Chạy độc lập là ưu điểm của link tĩnh. Rủi ro của thư viện động: thiếu hoặc sai phiên bản ở máy đích, và một bản cập nhật lỗi làm hỏng mọi chương trình dùng nó.

### Hàm nào lấy địa chỉ của symbol `my_function` sau khi đã `dlopen`?
- [ ] `dlopen`
- [x] `dlsym`
- [ ] `dlerror`
- [ ] `dladdr`
> `void *h = dlopen("libtest.so", RTLD_LAZY); void (*f)() = dlsym(h, "my_function"); (*f)(); dlclose(h);`. Lỗi thì đọc bằng `dlerror()`.

### Vì sao hàm C++ muốn lấy bằng dlsym phải khai báo `extern "C"`?
- [ ] Để chạy nhanh hơn
- [x] Để tên symbol không bị name mangling, dlsym tìm được đúng chuỗi "my_function"
- [ ] Vì dlsym chỉ chạy được với C
- [ ] Để hàm thành static
> g++ mã hoá kiểu tham số vào tên (ví dụ `_Z11my_functionv`). extern "C" giữ nguyên tên kiểu C.
