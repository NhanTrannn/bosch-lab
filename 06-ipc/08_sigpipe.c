/*
 * Bài: Ghi vào pipe không còn đầu đọc: SIGPIPE và EPIPE
 * Chủ đề: IPC
 * Dự đoán trước khi chạy: Khi bỏ qua SIGPIPE, write trả về gì? Khi để mặc định, tiến trình con kết thúc thế nào? Dung lượng pipe mặc định trên Linux?
 * Ý chính: Ghi vào pipe/socket mà mọi đầu đọc đã đóng → kernel gửi SIGPIPE (mặc định giết tiến trình). Nếu bỏ qua SIGPIPE thì write trả -1, errno = EPIPE. Ngược lại, đọc pipe khi mọi đầu ghi đã đóng → read trả 0 (EOF); quên đóng một đầu ghi → read chặn mãi.
 * Thử thêm: Chạy yes | head -1 rồi echo ${PIPESTATUS[@]} — yes chết vì SIGPIPE (141 = 128 + 13).
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pipe(fd);
    printf("dung luong pipe = %d byte\n", fcntl(fd[1], F_GETPIPE_SZ));
    close(fd[0]); /* không còn ai đọc */

    signal(SIGPIPE, SIG_IGN);
    ssize_t n = write(fd[1], "x", 1);
    printf("bo qua SIGPIPE: write = %zd, errno = %s\n", n, strerror(errno));

    pid_t pid = fork();
    if (pid == 0) {
        signal(SIGPIPE, SIG_DFL);
        write(fd[1], "x", 1);
        printf("con: khong in\n");
        _exit(0);
    }
    int st;
    waitpid(pid, &st, 0);
    if (WIFSIGNALED(st))
        printf("con bi kill boi signal %d (%s)\n", WTERMSIG(st), strsignal(WTERMSIG(st)));

    int p2[2];
    pipe(p2);
    write(p2[1], "hi", 2);
    close(p2[1]);
    char buf[8];
    ssize_t a = read(p2[0], buf, sizeof(buf));
    ssize_t b = read(p2[0], buf, sizeof(buf));
    printf("read lan 1 = %zd, read lan 2 = %zd (EOF)\n", a, b);
    return 0;
}
