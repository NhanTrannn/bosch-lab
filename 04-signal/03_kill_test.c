/*
 * !!! BÀI TREO CỐ Ý (vòng lặp vô hạn): nhấn Ctrl+C để thoát. Cần 2 terminal.
 * Bài: Gửi signal bằng lệnh kill
 * Chủ đề: Signal
 * Dự đoán trước khi chạy: Dòng đầu tiên in gì (sigaction cho SIGKILL)? Với từng lệnh kill -TERM, -USR1, -STOP, -CONT, -KILL từ terminal 2: chương trình phản ứng ra sao?
 * Ý chính: SIGKILL (9) và SIGSTOP (19) không thể bắt, chặn hay bỏ qua. SIGTERM (15) là yêu cầu "lịch sự", có thể bắt.
 * Thử thêm: Nhấn Ctrl+Z trong terminal 1 (SIGTSTP), rồi gõ fg. So sánh với kill -STOP.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

static void handler(int sig)
{
    const char *msg = sig == SIGTERM ? "[handler] SIGTERM -> van chay tiep\n"
                    : sig == SIGUSR1 ? "[handler] SIGUSR1\n"
                    : "[handler] signal khac\n";
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGKILL, &sa, NULL) == -1)
        perror("sigaction(SIGKILL)");
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    pid_t me = getpid();
    printf("PID = %d. Terminal 2 thu lan luot:\n", me);
    printf("  kill -TERM %d ; kill -USR1 %d\n", me, me);
    printf("  kill -STOP %d ; kill -CONT %d ; kill -KILL %d\n", me, me, me);

    for (int i = 0;; i++) {
        printf("tick %d\n", i);
        sleep(2);
    }
}
