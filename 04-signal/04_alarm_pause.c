/*
 * Bài: alarm, pause và hành động mặc định của SIGALRM
 * Chủ đề: Signal
 * Dự đoán trước khi chạy: alarm(0) trả về mấy? pause() trả về gì và errno là gì? Dòng cuối "khong in" có xuất hiện không, và shell báo gì?
 * Ý chính: alarm(n) hẹn gửi SIGALRM sau n giây (mỗi tiến trình chỉ có một hẹn giờ, gọi lại sẽ thay thế và trả về số giây còn lại). pause() ngủ tới khi có signal được xử lý, luôn trả -1/EINTR. Không bắt SIGALRM → hành động mặc định là kết thúc tiến trình ("Alarm clock").
 * Thử thêm: Dùng sigaction với SA_RESTART và read() từ stdin thay cho pause() — read có bị ngắt không?
 */
#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void on_alarm(int sig)
{
    (void)sig;
    const char msg[] = "  [handler] SIGALRM\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(void)
{
    alarm(10);
    unsigned left = alarm(0); /* huỷ hẹn giờ cũ */
    printf("alarm(0) tra ve %u\n", left);

    signal(SIGALRM, on_alarm);
    printf("alarm(1) roi pause()...\n");
    alarm(1);
    int r = pause();
    printf("pause() = %d, errno = %s\n", r, strerror(errno));

    signal(SIGALRM, SIG_DFL);
    printf("alarm(1) lan nua, lan nay KHONG bat SIGALRM\n");
    alarm(1);
    pause();
    printf("khong in\n");
    return 0;
}
