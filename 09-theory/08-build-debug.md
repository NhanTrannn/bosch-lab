# Build, link và debug

## Lý thuyết

### Các bước biên dịch
| Bước | Lệnh | Đầu ra |
| --- | --- | --- |
| Preprocess | `gcc -E a.c` | .i (đã thay #include, #define) |
| Compile | `gcc -S a.c` | .s (assembly) |
| Assemble | `gcc -c a.c` | .o (object, mã máy chưa liên kết) |
| Link | `gcc a.o b.o -o app` | file thực thi ELF |

- Lỗi cú pháp/kiểu → ở bước compile. `undefined reference to 'f'` → ở bước **link** (khai báo có nhưng định nghĩa không được link vào). `multiple definition` → định nghĩa trùng ở hai file.
- Cờ hay dùng: `-Wall -Wextra` (cảnh báo), `-g` (thông tin debug), `-O0/-O2/-O3/-Os` (tối ưu), `-I dir` (thư mục header), `-L dir -lname` (thư mục và thư viện libname), `-D NAME=val`, `-std=c11`, `-static`, `-fPIC`, `-fsanitize=address,undefined`.

### Thư viện
- **Tĩnh** `.a` (archive các .o, tạo bằng `ar rcs libx.a a.o b.o`): copy vào file thực thi lúc link → file lớn hơn, không phụ thuộc lúc chạy, cập nhật thư viện phải link lại.
- **Động** `.so` (`gcc -shared -fPIC -o libx.so a.c`): nạp lúc chạy bởi dynamic linker (`ld-linux.so`); nhiều chương trình dùng chung trong RAM, cập nhật không cần build lại. Tìm theo `LD_LIBRARY_PATH`, `/etc/ld.so.conf` (`ldconfig`), rpath. Xem phụ thuộc: `ldd app`.
- Thứ tự `-l` quan trọng với thư viện tĩnh: đặt thư viện **sau** file dùng nó.
- `extern "C"` khi gọi hàm C từ C++ (tắt name mangling).

### ELF và công cụ binutils
- Section: `.text` (code), `.rodata`, `.data`, `.bss`, `.symtab`, `.debug_*`.
- `file` (loại file), `size` (kích thước text/data/bss), `nm` (symbol: T text, D data, B bss, U undefined), `objdump -d` (disassemble), `readelf -h/-S/-s`, `strings`, `strip` (bỏ symbol), `addr2line`.

### Make
```make
CC = gcc
CFLAGS = -Wall -g
app: main.o util.o
	$(CC) $^ -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY: clean
clean:
	rm -f *.o app
```
- Rule: `target: prerequisites` + lệnh (bắt đầu bằng **TAB**). Chỉ build lại khi prerequisite mới hơn target.
- `$@` target, `$<` prerequisite đầu tiên, `$^` mọi prerequisite. `.PHONY` cho target không phải file. `=` gán trễ, `:=` gán ngay. `make -j4` song song.
- CMake sinh ra Makefile/Ninja: `cmake -B build && cmake --build build`.

### Debug
- **gdb**: `gdb ./app` → `break main`/`b file.c:20`, `run args`, `next` (n, bước qua hàm), `step` (s, vào hàm), `continue` (c), `finish`, `print x`, `display x`, `watch x` (dừng khi x đổi), `backtrace` (bt), `frame n`, `info locals`, `x/4xw addr` (xem bộ nhớ), `set var x = 5`, `quit`. Core dump: `ulimit -c unlimited` rồi `gdb ./app core`. Gắn vào process đang chạy: `gdb -p PID`. Từ xa (embedded): `gdbserver` + `target remote`.
- **valgrind** (memcheck): rò rỉ bộ nhớ, đọc/ghi ngoài vùng, dùng sau free, biến chưa khởi tạo. `valgrind --leak-check=full ./app`.
- **AddressSanitizer** `-fsanitize=address`: nhanh hơn valgrind nhiều, cần biên dịch lại.
- `strace` (system call), `ltrace` (hàm thư viện), `perf` (hiệu năng), `objdump`.
- Cross-compile cho embedded: toolchain như `arm-linux-gnueabihf-gcc`, `aarch64-linux-gnu-gcc`; build trên host (x86) chạy trên target (ARM).

### Git cơ bản
`clone`, `status`, `add`, `commit`, `log --oneline`, `diff`, `branch`, `checkout`/`switch`, `merge`, `rebase`, `pull` (= fetch + merge), `push`, `stash`, `reset --soft/--mixed/--hard`, `revert` (tạo commit đảo ngược, an toàn với lịch sử đã push), `cherry-pick`, `tag`.

## Trắc nghiệm

### Lỗi `undefined reference to 'foo'` xảy ra ở giai đoạn nào?
- [ ] Preprocess
- [ ] Compile
- [ ] Assemble
- [x] Link
> Compiler chấp nhận vì có khai báo, nhưng linker không tìm thấy định nghĩa (quên file .c hoặc quên -lthu_vien).

### Tuỳ chọn gcc nào chỉ dịch sang file object (.o) mà không link?
- [ ] -E
- [ ] -S
- [x] -c
- [ ] -o
> -E dừng sau preprocess, -S dừng sau compile (ra .s), -o đặt tên đầu ra.

### Ưu điểm của thư viện động (.so) so với thư viện tĩnh (.a)? (chọn tất cả đáp án đúng)
- [x] File thực thi nhỏ hơn
- [x] Nhiều tiến trình dùng chung một bản trong RAM
- [x] Cập nhật thư viện không cần link lại chương trình
- [ ] Không cần thư viện có mặt khi chạy
> Nhược điểm: phải có đúng phiên bản .so khi chạy, khởi động chậm hơn chút.

### Lệnh nào liệt kê các thư viện động mà chương trình phụ thuộc?
- [ ] nm app
- [x] ldd app
- [ ] objdump -h app
- [ ] file app
> nm liệt kê symbol, file cho biết loại file (ELF, dynamically linked...).

### Trong output của `nm`, chữ `U` trước một symbol nghĩa là?
- [ ] Unused
- [x] Undefined — được dùng nhưng định nghĩa ở nơi khác
- [ ] Unsigned
- [ ] User-defined
> T: hàm trong .text, D: biến trong .data, B: biến trong .bss, R: rodata. Chữ thường = symbol local (static).

### Trong Makefile, `$@` và `$<` là gì?
- [x] Tên target và prerequisite đầu tiên
- [ ] Tất cả prerequisite và tên target
- [ ] Thư mục hiện tại và tên file
- [ ] Tên compiler và cờ biên dịch
> $^ là toàn bộ prerequisite.

### Dòng lệnh trong rule của Makefile phải bắt đầu bằng gì?
- [ ] 4 dấu cách
- [x] Ký tự TAB
- [ ] Dấu @
- [ ] Dấu -
> Dùng dấu cách sẽ báo lỗi "missing separator". @ (ẩn lệnh) và - (bỏ qua lỗi) chỉ là tiền tố tuỳ chọn sau TAB.

### Vì sao cần `.PHONY: clean`?
- [ ] Để clean chạy nhanh hơn
- [x] Để make luôn chạy rule clean kể cả khi có file tên "clean"
- [ ] Để clean chạy trước mọi rule
- [ ] Để make không in lệnh ra
> Nếu không, có file tên clean thì make cho là target đã cập nhật và bỏ qua.

### Trong gdb, khác biệt giữa `next` và `step`?
- [ ] Không khác
- [x] next chạy qua cả lời gọi hàm, step đi vào bên trong hàm
- [ ] step chạy tới breakpoint tiếp theo
- [ ] next lùi lại một dòng
> finish chạy tới khi hàm hiện tại trả về; continue chạy tới breakpoint kế tiếp.

### Lệnh gdb nào dừng chương trình khi giá trị biến `count` thay đổi?
- [ ] break count
- [x] watch count
- [ ] display count
- [ ] print count
> Watchpoint (phần cứng nếu có thể). display in giá trị mỗi lần dừng.

### Lệnh gdb để xem call stack khi chương trình crash?
- [ ] stack
- [x] backtrace (bt)
- [ ] info frame all
- [ ] where-is
> `frame n` để chuyển tới khung thứ n, `info locals` xem biến local. `where` là tên khác của backtrace.

### Công cụ nào phát hiện memory leak mà KHÔNG cần biên dịch lại?
- [x] valgrind
- [ ] -fsanitize=address
- [ ] gcc -Wall
- [ ] gdb watch
> ASan cần biên dịch lại với cờ -fsanitize=address nhưng chạy nhanh hơn valgrind nhiều.

### Cross-compilation nghĩa là gì?
- [ ] Biên dịch C và C++ cùng lúc
- [x] Biên dịch trên một kiến trúc (host, ví dụ x86) để chạy trên kiến trúc khác (target, ví dụ ARM)
- [ ] Biên dịch bằng nhiều compiler
- [ ] Biên dịch song song nhiều file
> Ví dụ `arm-linux-gnueabihf-gcc main.c -o main` trên PC rồi chép sang board.

### `extern "C"` trong code C++ dùng để làm gì?
- [ ] Biên dịch đoạn đó bằng compiler C
- [x] Tắt name mangling để link được với hàm viết bằng C
- [ ] Khai báo biến toàn cục
- [ ] Cho phép dùng malloc
> Header C thường bọc trong `#ifdef __cplusplus extern "C" { #endif`.

### Lệnh git nào huỷ một commit đã push mà không viết lại lịch sử?
- [ ] git reset --hard HEAD~1
- [x] git revert <commit>
- [ ] git checkout HEAD~1
- [ ] git commit --amend
> revert tạo commit mới đảo ngược thay đổi. reset/amend viết lại lịch sử, phải force push.

### `git pull` tương đương với?
- [ ] git clone + git merge
- [x] git fetch + git merge (hoặc rebase nếu cấu hình)
- [ ] git push + git fetch
- [ ] git checkout + git reset
> `git pull --rebase` dùng rebase thay vì merge.

### Biến `static int x;` ở mức file trong a.c và `int x;` trong b.c, link chung thì?
- [ ] Lỗi multiple definition
- [x] Hai biến khác nhau, không xung đột
- [ ] Hai tên trỏ cùng một biến
- [ ] Lỗi undefined reference
> static cho internal linkage nên x trong a.c không lộ ra ngoài.
