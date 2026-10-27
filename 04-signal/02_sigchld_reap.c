/*
 * Bài: Dọn zombie bằng SIGCHLD
 * Chủ đề: Signal
 * Dự đoán trước khi chạy: Chương trình có in đủ "Da don du 5 con" không? Nếu handler chỉ gọi waitpid một lần (không có while) thì có thể sai ở đâu?
 * Ý chính: Khi con thoát, kernel gửi SIGCHLD cho cha. Signal thường KHÔNG xếp hàng: nhiều con thoát gần nhau có thể chỉ sinh 1 lần gọi handler → phải lặp waitpid(-1, ..., WNOHANG).
 * Thử thêm: Bỏ vòng while trong handler (chỉ gọi waitpid 1 lần), cho các con thoát cùng lúc (bỏ usleep) và chạy vài lần.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

static volatile sig_atomic_t reaped = 0;

static void on_sigchld(int sig)
{
    (void)sig;
    int saved = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        reaped++;
    errno = saved;
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    for (int i = 0; i < 5; i++) {
        if (fork() == 0) {
            usleep(100000 * (i + 1));
            _exit(i);
        }
    }

    while (reaped < 5) {
        printf("cha dang lam viec... da don %d con\n", (int)reaped);
        sleep(1);
    }
    printf("Da don du %d con, khong con zombie\n", (int)reaped);
    return 0;
}
