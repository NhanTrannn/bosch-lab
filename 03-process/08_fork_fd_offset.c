/*
 * Bài: File descriptor sau fork(): dùng chung offset
 * Chủ đề: Process
 * Dự đoán trước khi chạy: File 1 chứa gì (cha và con dùng chung fd mở TRƯỚC fork)? File 2 chứa gì (mỗi bên tự open SAU fork)?
 * Ý chính: fork() nhân bản bảng fd, nhưng các fd trỏ tới CÙNG một "open file description" trong kernel → dùng chung offset và cờ. open() riêng lẻ tạo description riêng → offset riêng, ghi đè lên nhau.
 * Thử thêm: Mở file 2 với O_APPEND ở cả hai bên rồi chạy lại. Dùng dup() thay vì fork để thấy hiệu ứng offset chung tương tự.
 */
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static void dump(const char *path)
{
    char buf[128] = {0};
    int fd = open(path, O_RDONLY);
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    printf("%s (%zd byte):\n%s", path, n, buf);
}

int main(void)
{
    const char *f1 = "/tmp/bosch_offset1.txt", *f2 = "/tmp/bosch_offset2.txt";

    int fd = open(f1, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fork() == 0) {
        write(fd, "con\n", 4);
        _exit(0);
    }
    wait(NULL);
    write(fd, "cha\n", 4);
    close(fd);
    dump(f1);

    if (fork() == 0) {
        int fd2 = open(f2, O_CREAT | O_TRUNC | O_WRONLY, 0644);
        write(fd2, "XXXXXXXX\n", 9);
        close(fd2);
        _exit(0);
    }
    wait(NULL);
    int fd2 = open(f2, O_WRONLY);
    write(fd2, "cha\n", 4);
    close(fd2);
    dump(f2);

    unlink(f1);
    unlink(f2);
    return 0;
}
