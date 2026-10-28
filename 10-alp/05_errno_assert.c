/*
 * Bài: errno, strerror, perror và assert (kể cả NDEBUG) (ALP 2.2)
 * Chủ đề: ALP chương 2
 * Dự đoán trước khi chạy: errno của từng lời gọi lỗi là gì? Sau một lời gọi THÀNH CÔNG, errno có về 0 không? Với NDEBUG, assert(do_something() == 0) gọi hàm mấy lần? Dòng cuối kết thúc chương trình thế nào?
 * Ý chính: System call lỗi trả về -1 (hoặc NULL) và đặt errno; thành công thì KHÔNG xoá errno → chỉ đọc errno ngay sau khi lời gọi báo lỗi, và lưu lại trước khi gọi hàm khác. strerror(errno) đổi mã thành chuỗi, perror in ra stderr. assert(expr) sai → in file:dòng rồi abort(); -DNDEBUG xoá hẳn assert nên biểu thức trong assert không được có tác dụng phụ.
 * !!! Bài cố ý kết thúc bằng SIGABRT ở dòng assert cuối: shell báo "Aborted (core dumped)", exit code 134.
 * Thử thêm: Đọc /usr/include/asm-generic/errno-base.h. Biên dịch lại với -DNDEBUG và xem dòng assert cuối có còn abort không.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int calls = 0;
static int do_something(void) { ++calls; return 0; }

static void report(const char *what, int ret)
{
    int saved = errno;   /* lưu ngay, printf có thể đổi errno */
    printf("%-28s -> %2d, errno = %2d %-7s (%s)\n", what, ret, saved,
           saved == ENOENT ? "ENOENT" : saved == EISDIR ? "EISDIR" : saved == EBADF ? "EBADF" :
           saved == EACCES ? "EACCES" : saved == EEXIST ? "EEXIST" : saved == EINVAL ? "EINVAL" : "",
           strerror(saved));
}

#define NDEBUG
#include <assert.h>
static void with_ndebug(void)
{
    for (int i = 0; i < 3; ++i) assert(do_something() == 0);
}

#undef NDEBUG
#include <assert.h>   /* assert.h được thiết kế để include lại, mỗi lần đọc lại NDEBUG */
static void without_ndebug(void)
{
    for (int i = 0; i < 3; ++i) assert(do_something() == 0);
}

int main(void)
{
    report("open(\"/khong/ton/tai\")", open("/khong/ton/tai", O_RDONLY));
    report("getpid() (thanh cong)", getpid() > 0);
    report("open(\"/tmp\", O_WRONLY)", open("/tmp", O_WRONLY));
    report("close(12345)", close(12345));
    report("mkdir(\"/tmp\")", mkdir("/tmp", 0755));
    report("lseek(0, 0, 12345)", (int)lseek(0, 0, 12345));
    if (geteuid() != 0) report("open(\"/etc/shadow\")", open("/etc/shadow", O_RDONLY));
    fflush(stdout);

    errno = ENOENT;
    perror("perror in ra stderr");

    with_ndebug();
    printf("NDEBUG: calls = %d\n", calls);
    without_ndebug();
    printf("khong NDEBUG: calls = %d\n", calls);
    fflush(stdout);

    const char *pointer = NULL;
    assert(pointer != NULL);
    printf("khong bao gio in\n");
    return 0;
}
