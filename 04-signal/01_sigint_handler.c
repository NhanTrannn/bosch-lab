/*
 * !!! Chạy liên tục cho tới khi nhấn Ctrl+C đủ 3 lần (hoặc Ctrl+\ để thoát ngay).
 * Bài: Bắt Ctrl+C bằng sigaction
 * Chủ đề: Signal
 * Dự đoán trước khi chạy: Nhấn Ctrl+C lần 1, 2 thì sao? Lần thứ mấy chương trình thoát, và dòng cuối in gì?
 * Ý chính: Handler chỉ nên đặt cờ kiểu volatile sig_atomic_t và dùng hàm async-signal-safe (write, không printf). Vòng lặp chính kiểm tra cờ để thoát sạch.
 * Thử thêm: Trong lúc chạy, từ terminal khác gửi kill -INT <PID>; rồi thử kill -QUIT <PID> (Ctrl+\).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t stop = 0;
static volatile sig_atomic_t count = 0;

static void on_sigint(int sig)
{
    (void)sig;
    count++;
    if (count >= 3)
        stop = 1;
    const char msg[] = " [handler: nhan SIGINT]\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    printf("PID %d. Nhan Ctrl+C 3 lan de thoat.\n", getpid());
    while (!stop) {
        printf("dang chay... count = %d\n", (int)count);
        sleep(1);
    }
    printf("Thoat sach se sau %d lan Ctrl+C\n", (int)count);
    return 0;
}
