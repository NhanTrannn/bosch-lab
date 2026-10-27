/*
 * Bài: Named pipe — phía ghi (đi cùng 03_fifo_reader.c)
 * Chủ đề: IPC
 * Dự đoán trước khi chạy: xem 03_fifo_reader.c
 * Ý chính: open(O_WRONLY) chặn cho tới khi có tiến trình mở FIFO để đọc.
 * Thử thêm: chạy writer trước reader và quan sát dòng nào bị chặn.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO "/tmp/bosch_fifo"

int main(void)
{
    if (mkfifo(FIFO, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }
    printf("Writer: dang mo FIFO...\n");
    fflush(stdout);
    int fd = open(FIFO, O_WRONLY);
    printf("Writer: da mo. Go vai dong roi Ctrl+D de ket thuc\n");

    char line[128];
    while (fgets(line, sizeof(line), stdin))
        write(fd, line, strlen(line));
    close(fd);
    printf("Writer: da dong FIFO\n");
    return 0;
}
