/*
 * Bài: Biến sau fork(): copy-on-write
 * Chủ đề: Process
 * Dự đoán trước khi chạy: Cha in g, local, heap bằng mấy sau khi con đã tăng cả ba? Địa chỉ &g ở cha và con có giống nhau không?
 * Ý chính: Sau fork(), con có bản sao RIÊNG của toàn bộ không gian địa chỉ (global, heap, stack). Địa chỉ ảo giống hệt nhau nhưng ánh xạ tới trang vật lý khác nhau khi có ghi (copy-on-write). Muốn chia sẻ dữ liệu phải dùng IPC (shm, pipe...).
 * Thử thêm: Dùng mmap(MAP_SHARED | MAP_ANONYMOUS) cho biến đếm rồi fork — lần này cha thấy thay đổi của con.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int g = 10;

int main(void)
{
    int local = 20;
    int *heap = malloc(sizeof(*heap));
    *heap = 30;

    pid_t pid = fork();
    if (pid == 0) {
        g++;
        local++;
        (*heap)++;
        printf("con: g=%d local=%d heap=%d | &g=%p heap=%p\n", g, local, *heap, (void *)&g, (void *)heap);
        free(heap);
        exit(0);
    }
    wait(NULL);
    printf("cha: g=%d local=%d heap=%d | &g=%p heap=%p\n", g, local, *heap, (void *)&g, (void *)heap);
    free(heap);
    return 0;
}
