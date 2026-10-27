/*
 * Bài: fork() ba lần
 * Chủ đề: Process
 * Dự đoán trước khi chạy: Có bao nhiêu dòng "PID = ..." được in? Có bao nhiêu PPID khác nhau?
 * Ý chính: Mỗi tiến trình đang chạy đều thực hiện fork() tiếp theo → số tiến trình nhân đôi sau mỗi lần fork: n lần fork tuần tự → 2^n tiến trình.
 * Thử thêm: Thay 3 lần fork bằng vòng for (i = 0; i < 3; i++) fork(); — kết quả có khác không? Còn for (i < 4)?
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    fork();
    fork();
    fork();
    printf("PID = %d, PPID = %d\n", getpid(), getppid());

    while (wait(NULL) > 0)      /* chờ hết con cho output gọn */
        ;
    return 0;
}
