/*
 * !!! Cần 2 terminal. Terminal 1: make run T=06-ipc/03_fifo_reader. Terminal 2: make run T=06-ipc/03_fifo_writer.
 * Bài: Named pipe (FIFO) giữa 2 chương trình
 * Chủ đề: IPC
 * Dự đoán trước khi chạy: Khi chỉ chạy reader, nó dừng ở dòng nào? Khi writer gõ Ctrl+D, reader in gì rồi kết thúc ra sao?
 * Ý chính: FIFO là pipe có tên trong filesystem, cho phép 2 tiến trình KHÔNG họ hàng giao tiếp. open() đầu đọc chặn cho tới khi có đầu ghi mở (và ngược lại).
 * Thử thêm: Chạy ls -l /tmp/bosch_fifo khi reader đang chờ: ký tự đầu tiên của quyền là gì? Thử echo hello > /tmp/bosch_fifo thay cho writer.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO "/tmp/bosch_fifo"

int main(void)
{
    if (mkfifo(FIFO, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }
    printf("Reader: dang mo FIFO...\n");
    fflush(stdout);
    int fd = open(FIFO, O_RDONLY);
    printf("Reader: da mo, cho du lieu\n");

    char buf[128];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("Reader nhan: %s", buf);
        fflush(stdout);
    }
    printf("Reader: read tra ve %zd, ket thuc\n", n);
    close(fd);
    unlink(FIFO);
    return 0;
}
