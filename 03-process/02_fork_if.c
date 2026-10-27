/*
 * Bài: fork() trong if và trong biểu thức logic
 * Chủ đề: Process
 * Dự đoán trước khi chạy: Phần [1] in mấy dòng? Phần [2] với fork() && fork() || fork() in mấy dòng?
 * Ý chính: fork() trả về 0 ở con, PID con (> 0) ở cha. && và || có short-circuit: vế phải chỉ chạy khi cần.
 * Thử thêm: Đoán số dòng của fork() || fork() && fork(); và (fork() && fork()) || (fork() && fork()).
 * Cảnh báo biên dịch cố ý: -Wparentheses và -Wunused-value ở dòng fork() && fork() || fork().
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

static void reap(void)
{
    while (wait(NULL) > 0)
        ;
}

int main(void)
{
    pid_t root = getpid();

    /* Phần 1 */
    if (fork() == 0)
        fork();
    printf("[1] pid = %d\n", getpid());
    reap();
    if (getpid() != root)
        return 0;               /* chỉ tiến trình gốc đi tiếp */

    printf("-----\n");
    fflush(stdout);

    /* Phần 2 */
    fork() && fork() || fork();
    printf("[2] pid = %d\n", getpid());
    reap();
    return 0;
}
