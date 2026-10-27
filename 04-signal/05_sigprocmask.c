/*
 * Bài: Chặn signal (sigprocmask), signal đang chờ và việc signal không xếp hàng
 * Chủ đề: Signal
 * Dự đoán trước khi chạy: Gửi SIGUSR1 3 lần khi đang chặn → bỏ chặn thì handler chạy mấy lần? Còn với signal real-time SIGRTMIN?
 * Ý chính: Signal bị chặn (blocked) thì nằm ở trạng thái pending, chưa được giao. Signal chuẩn (1–31) chỉ là MỘT bit pending → gửi nhiều lần khi đang chặn chỉ nhận một lần. Signal real-time (SIGRTMIN..SIGRTMAX) được xếp hàng, nhận đủ số lần.
 * Thử thêm: Chặn SIGINT, ngủ 5 giây và nhấn Ctrl+C liên tục; sau khi bỏ chặn chương trình mới chết. Thử chặn SIGKILL — sigprocmask lặng lẽ bỏ qua.
 */
#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>

static volatile sig_atomic_t usr1, rt;

static void handler(int sig)
{
    if (sig == SIGUSR1)
        usr1++;
    else
        rt++;
}

int main(void)
{
    signal(SIGUSR1, handler);
    signal(SIGRTMIN, handler);

    sigset_t set, pending;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &set, NULL);

    for (int i = 0; i < 3; i++) {
        raise(SIGUSR1);
        raise(SIGRTMIN);
    }

    sigpending(&pending);
    printf("dang chan: SIGUSR1 pending? %d, SIGRTMIN pending? %d\n",
           sigismember(&pending, SIGUSR1), sigismember(&pending, SIGRTMIN));
    printf("truoc khi bo chan: usr1 = %d, rt = %d\n", usr1, rt);

    sigprocmask(SIG_UNBLOCK, &set, NULL);
    printf("sau khi bo chan:   usr1 = %d, rt = %d\n", usr1, rt);
    return 0;
}
