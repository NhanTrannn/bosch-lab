# 08-shell — 20 bài tập lệnh

Gõ thật trên terminal Ubuntu, ghi lại lệnh và kết quả vào chỗ trống. Đáp án ở `ANSWERS.md` (phần 08-shell).

## Drill 01

Tạo file `run.sh`, đặt quyền `754`, kiểm tra bằng `ls -l` và `stat`. Chuỗi quyền hiển thị là gì? Ai được làm gì?

> Gợi ý: Mỗi chữ số bát phân = tổng r(4) + w(2) + x(1).

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 02

Xem `umask` hiện tại. Với `umask 022`, file mới tạo bằng `touch` và thư mục mới tạo bằng `mkdir` có quyền gì?

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 03

Tạo `a.txt`, tạo hard link `hard.txt` và soft link `soft.txt` tới nó. Xoá `a.txt`. `cat` hai link cho kết quả gì? `ls -li` cho thấy gì về inode?

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 04

Dùng `ls /etc/hostname /khong_co` (vừa in stdout vừa in stderr). So sánh `cmd > f 2>&1` với `cmd 2>&1 > f`: lỗi xuất hiện ở đâu?

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 05

Phân biệt `>`, `>>`, `2>/dev/null`, `&>` và `tee`. Ghi output của `date` vào `log.txt` đồng thời vẫn thấy trên màn hình, và không ghi đè nội dung cũ.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 06

Trong thư mục `bosch-lab`, tìm mọi dòng chứa `fork` trong các file `.c`, hiện số dòng. Sau đó: đếm số file chứa `pthread`, tìm không phân biệt hoa thường, và in các dòng KHÔNG chứa `#include` của một file.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 07

Dùng `find`: tìm mọi file `.cpp`; tìm file lớn hơn 1MB trong `/var/log`; tìm file sửa trong 1 ngày qua; xoá tất cả file trong `build/` có đuôi `.o`.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 08

Dùng `awk` in cột 1 (tên user) của `/etc/passwd`. In tên user và shell (cột 7) của những user có UID ≥ 1000.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 09

Đếm mỗi loại shell (cột 7 của `/etc/passwd`) được bao nhiêu user dùng, sắp xếp giảm dần.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 10

Đếm số dòng, số từ, số byte của `/etc/passwd`. Đếm số file `.c` trong `bosch-lab`.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 11

In 5 dòng đầu, 5 dòng cuối, và từ dòng 2 trở đi của `/etc/passwd`. Theo dõi log realtime.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 12

Tìm tiến trình `bash` đang chạy bằng `ps aux | grep` và bằng `pgrep`. Ý nghĩa các cột `VSZ`, `RSS`, `STAT` của `ps aux`?

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 13

Mở `top`: sắp xếp theo RAM, theo CPU, xem từng nhân, thoát. `load average` 3 con số nghĩa là gì?

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 14

Chạy `sleep 100`, nhấn `Ctrl+Z`, đưa nó chạy nền, liệt kê job, đưa về foreground, rồi kết thúc nó. Chạy thẳng một lệnh ở chế độ nền.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 15

Liệt kê tên các signal. Khác nhau giữa `kill PID`, `kill -9 PID`, `pkill`, `killall`?

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 16

So sánh `df -h` và `du -sh`. Thư mục nào trong `/var` chiếm nhiều chỗ nhất?

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 17

Chạy `make run T=03-process/05_zombie` ở terminal 1. Ở terminal 2, xem các file mà tiến trình đó đang mở bằng `lsof` và `/proc`. Xem tiến trình nào đang nghe cổng 22.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 18

Dùng `strace` xem các system call của `01_fork_count`: thống kê số lần gọi, và chỉ xem `clone`/`write`. Dùng `ltrace` cho lời gọi thư viện.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 19

In mã thoát của lệnh trước, PID của shell hiện tại, PID của job nền gần nhất. Thêm `~/bin` vào PATH. Khác nhau giữa biến shell và biến môi trường?

```sh
# lệnh của bạn:


# kết quả quan sát được:

```

## Drill 20

Quan sát 4 giai đoạn biên dịch của `01-c/08_macro_traps.c`. Xem thư viện động mà file thực thi dùng, và các symbol trong file `.o`.

```sh
# lệnh của bạn:


# kết quả quan sát được:

```
