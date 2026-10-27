# Linux: lệnh, quyền và filesystem

## Lý thuyết

### Cây thư mục (FHS)
`/bin`, `/usr/bin` lệnh người dùng · `/sbin` lệnh quản trị · `/etc` cấu hình · `/home` thư mục người dùng · `/root` của root · `/var` dữ liệu thay đổi (log: `/var/log`) · `/tmp` tạm · `/dev` file thiết bị · `/proc` thông tin process/kernel (ảo) · `/sys` thiết bị và driver (sysfs, ảo) · `/boot` kernel, initramfs · `/lib` thư viện, module kernel (`/lib/modules`) · `/opt` phần mềm thêm · `/mnt`, `/media` điểm mount.

### Quyền truy cập
- `-rwxr-xr--`: ký tự đầu là loại file (`-` thường, `d` thư mục, `l` symlink, `c` char device, `b` block device, `p` FIFO, `s` socket), sau đó 3 nhóm owner/group/others.
- r = 4, w = 2, x = 1 → `chmod 754 f` = rwxr-xr--. `chmod u+x,g-w f`. `chown user:group f`.
- Với thư mục: r = liệt kê tên, w = tạo/xoá file trong đó, x = đi vào (cd) và truy cập file bên trong.
- `umask 022` → file mới 644 (666 & ~022), thư mục mới 755.
- Bit đặc biệt: **setuid** (4xxx, chạy với quyền owner, ví dụ `passwd`), **setgid** (2xxx), **sticky** (1xxx, trên `/tmp`: chỉ owner xoá được file của mình).
- Mọi thứ là file; quyền kiểm tra theo UID/GID của process.

### Inode và link
- **Inode** lưu metadata (quyền, owner, kích thước, thời gian, con trỏ tới block dữ liệu) — **không** lưu tên. Tên file nằm trong thư mục (ánh xạ tên → inode).
- **Hard link** (`ln a b`): thêm tên trỏ cùng inode, tăng link count; xoá một tên không mất dữ liệu. Không tạo được cho thư mục và không vượt filesystem.
- **Soft/symbolic link** (`ln -s a b`): file riêng chứa đường dẫn; xoá file gốc → link hỏng (dangling). Vượt filesystem, trỏ được thư mục.
- `ls -li` xem inode, `stat f` xem chi tiết, `df -i` xem số inode còn lại.

### File descriptor và redirect
- 0 stdin, 1 stdout, 2 stderr. `>` ghi đè, `>>` nối, `<` đọc, `2>` stderr, `2>&1` stderr theo stdout, `&>` cả hai.
- Thứ tự quan trọng: `cmd > f 2>&1` (cả hai vào f) khác `cmd 2>&1 > f` (stderr ra terminal).
- `|` nối stdout lệnh trước với stdin lệnh sau. `tee` vừa ghi file vừa in.
- `/dev/null` bỏ output. `$?` mã thoát lệnh trước (0 = thành công). `&&` chạy tiếp nếu thành công, `||` nếu thất bại.

### Lệnh hay dùng
- Tìm: `find / -name "*.c" -size +1M -mtime -7`, `grep -rn "pattern" dir` (`-i` không phân biệt hoa thường, `-v` đảo, `-c` đếm, `-l` chỉ tên file, `-E` regex mở rộng), `which`, `locate`.
- Xử lý văn bản: `cat`, `head -n`, `tail -n`/`tail -f`, `wc -l`, `sort`, `uniq -c` (cần sort trước), `cut -d: -f1`, `awk -F: '{print $1}'`, `sed 's/a/b/g'`, `tr`, `diff`, `less`.
- Process: `ps`, `top`, `kill`, `pkill`, `pgrep`, `jobs`, `fg`, `bg`, `nohup`, `&`.
- Hệ thống: `df -h` (dung lượng filesystem), `du -sh` (dung lượng thư mục), `free -h`, `uname -r`, `lsblk`, `mount`, `dmesg`, `journalctl`, `systemctl status|start|enable svc`, `lsof` (file đang mở), `ss -tlnp`/`netstat` (cổng đang nghe), `ip addr`.
- Archive: `tar -czvf a.tar.gz dir` (tạo), `tar -xzvf a.tar.gz` (giải nén).
- Gói (Ubuntu): `apt install/update/upgrade`, `dpkg -l`.
- Biến môi trường: `export VAR=x`, `echo $PATH`, `env`. Script bắt đầu bằng shebang `#!/bin/bash`; `$0` tên script, `$1..` đối số, `$#` số đối số, `$@` tất cả đối số, `$$` PID.

### Quá trình boot (x86 Linux)
BIOS/UEFI → bootloader (GRUB) → nạp kernel + initramfs → kernel khởi tạo, mount root filesystem → chạy **init** (PID 1, thường là systemd) → các service/target → login.
Trên embedded: ROM bootloader → U-Boot → kernel + device tree → rootfs → init.

## Trắc nghiệm

### `chmod 640 file` cho quyền gì?
- [ ] rwxr-----
- [x] rw-r-----
- [ ] rw-rw----
- [ ] r--r-----
> 6 = rw- (owner), 4 = r-- (group), 0 = --- (others).

### Với `umask 022`, file mới tạo bằng `touch` có quyền gì?
- [ ] 755
- [x] 644
- [ ] 666
- [ ] 600
> Quyền gốc của file là 666, bỏ đi bit trong umask → 644. Thư mục: 777 → 755.

### Quyền x trên một thư mục cho phép điều gì?
- [ ] Liệt kê tên file trong thư mục
- [x] cd vào thư mục và truy cập file bên trong theo tên
- [ ] Chạy mọi file trong thư mục
- [ ] Xoá thư mục
> r cho phép liệt kê tên, w cho phép tạo/xoá file (cần cả x).

### Phát biểu nào đúng về hard link? (chọn tất cả đáp án đúng)
- [x] Trỏ tới cùng inode với file gốc
- [x] Xoá file gốc thì dữ liệu vẫn truy cập được qua hard link
- [ ] Có thể tạo hard link cho thư mục bằng ln
- [ ] Có thể trỏ sang filesystem khác
> Dữ liệu chỉ bị xoá khi link count về 0 và không còn process nào mở file.

### Inode KHÔNG lưu thông tin nào?
- [ ] Quyền truy cập
- [ ] Kích thước file
- [x] Tên file
- [ ] Thời gian sửa đổi
> Tên nằm trong entry của thư mục. Đó là lý do một inode có thể có nhiều tên (hard link).

### Lệnh nào đưa cả stdout và stderr vào file out.txt?
- [ ] `cmd 2>&1 > out.txt`
- [x] `cmd > out.txt 2>&1`
- [ ] `cmd > out.txt 1>&2`
- [ ] `cmd 2> out.txt`
> Redirect xử lý từ trái sang phải: trước tiên stdout → file, sau đó stderr → nơi stdout đang trỏ (file). Viết ngược thì stderr trỏ vào terminal cũ.

### Ký tự đầu tiên `c` trong `ls -l` (ví dụ `crw-rw----`) nghĩa là?
- [ ] Thư mục cấu hình
- [x] Character device
- [ ] File nén
- [ ] Socket
> b là block device (đĩa), p là FIFO, s là socket, l là symlink, d là thư mục.

### Lệnh nào đếm số dòng chứa "error" (không phân biệt hoa thường) trong log.txt?
- [ ] `grep -v error log.txt | wc -l`
- [x] `grep -ci error log.txt`
- [ ] `grep -l error log.txt`
- [ ] `wc -l error log.txt`
> -c đếm số dòng khớp, -i không phân biệt hoa thường.

### Lệnh nào in cột thứ nhất (tên người dùng) của /etc/passwd?
- [x] `cut -d: -f1 /etc/passwd`
- [ ] `cut -f1 /etc/passwd`
- [ ] `awk '{print $1}' /etc/passwd`
- [ ] `sed -n 1p /etc/passwd`
> Các trường cách nhau bởi dấu `:`. Cũng dùng được `awk -F: '{print $1}' /etc/passwd`.

### Lệnh tìm mọi file .log lớn hơn 100MB trong /var?
- [ ] `grep -r "*.log" /var --size 100M`
- [x] `find /var -name "*.log" -size +100M`
- [ ] `ls -R /var | grep log`
- [ ] `locate /var/*.log > 100M`
> find hỗ trợ -name, -type f/d, -size, -mtime, -user, -exec.

### `$?` trong bash chứa gì?
- [ ] PID của shell
- [x] Mã thoát của lệnh vừa chạy
- [ ] Số đối số truyền vào script
- [ ] PID của lệnh chạy nền gần nhất
> 0 = thành công. `$$` là PID của shell, `$#` số đối số, `$!` PID tiến trình nền gần nhất.

### Sticky bit trên thư mục /tmp có tác dụng gì?
- [ ] File trong /tmp không bị xoá khi khởi động lại
- [x] Chỉ owner của file (hoặc root) mới xoá/đổi tên được file đó, dù ai cũng ghi được vào thư mục
- [ ] File chạy với quyền của owner
- [ ] Thư mục chỉ đọc
> Hiển thị là `t` ở vị trí cuối: `drwxrwxrwt`.

### Bit setuid trên `/usr/bin/passwd` để làm gì?
- [ ] Ngăn người dùng thường chạy passwd
- [x] Chương trình chạy với quyền của owner (root) để sửa được /etc/shadow
- [ ] Cho phép mọi người đọc /etc/shadow
- [ ] Ghi log mỗi lần chạy
> Hiển thị `s` thay cho x của owner: `-rwsr-xr-x`.

### Lệnh nào xem dung lượng còn trống của các filesystem?
- [x] df -h
- [ ] du -sh
- [ ] free -h
- [ ] lsblk -f
> du đo dung lượng thư mục/file đã dùng, free đo RAM.

### Lệnh nào theo dõi một file log khi nó đang được ghi thêm?
- [ ] cat -f
- [ ] head -f
- [x] tail -f
- [ ] less -n
> `less +F` cũng làm được. `journalctl -f` cho log của systemd.

### Tiến trình đầu tiên ở user space (PID 1) trên đa số bản Linux hiện nay là?
- [ ] kernel
- [x] systemd (init)
- [ ] bash
- [ ] grub
> Bootloader (GRUB/U-Boot) nạp kernel, kernel chạy init.

### Lệnh nào xem các cổng TCP đang lắng nghe kèm tiến trình?
- [ ] ps -tcp
- [x] ss -tlnp
- [ ] ip route
- [ ] ping -l
> Hoặc netstat -tlnp (cũ hơn). -t TCP, -l listen, -n số, -p process.

### Thư mục nào chứa thông tin về thiết bị và driver dưới dạng filesystem ảo?
- [ ] /dev
- [x] /sys
- [ ] /etc
- [ ] /boot
> /sys (sysfs) phản ánh mô hình thiết bị của kernel. /dev chứa file thiết bị để truy cập, /proc chứa thông tin process.

### `sort data.txt | uniq -c | sort -rn | head -3` làm gì?
- [ ] Xoá 3 dòng trùng đầu tiên
- [x] In 3 dòng xuất hiện nhiều nhất kèm số lần
- [ ] In 3 dòng đầu sau khi sắp xếp
- [ ] Đếm số dòng của file
> uniq chỉ gộp các dòng trùng **liền nhau** nên phải sort trước; sort -rn sắp số giảm dần.
