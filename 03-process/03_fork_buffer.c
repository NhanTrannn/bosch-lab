/*
 * Bài: Buffer của stdio khi fork
 * Chủ đề: Process
 * Dự đoán trước khi chạy: Màn hình hiện gì? Chữ A xuất hiện mấy lần?
 * Ý chính: printf ghi vào buffer trong user space; fork() nhân bản cả buffer chưa flush. stdout ra terminal là line-buffered, ra file/pipe là fully-buffered.
 * Thử thêm: Thêm fflush(stdout); ngay sau printf("A"). Sau đó đổi thành printf("A
") và chạy cả ./build/03-process/03_fork_buffer > out.txt; cat out.txt.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    printf("A");
    fork();
    printf("B\n");
    wait(NULL);
    return 0;
}
