/*
 * Bài: wait/waitpid và các macro trạng thái, WNOHANG, zombie trong /proc (ALP 3.4)
 * Chủ đề: ALP chương 3
 * Dự đoán trước khi chạy: exit(300) cho WEXITSTATUS bao nhiêu? abort() và kill -9 thì WIFSIGNALED/WTERMSIG là gì? waitpid(..., WNOHANG) trả về gì khi con chưa xong? Con đã thoát nhưng chưa được wait thì trạng thái trong /proc là gì? wait() khi không còn con?
 * Ý chính: Chỉ 8 bit thấp của exit code được giữ. WIFEXITED/WEXITSTATUS cho thoát bình thường, WIFSIGNALED/WTERMSIG cho bị signal giết. WNOHANG không chặn, trả về 0 nếu chưa có con nào kết thúc. Con kết thúc mà cha chưa wait → zombie (trạng thái Z), chỉ còn entry trong bảng process; wait xong thì biến mất.
 * Thử thêm: Trong lúc bài đang ngủ, ở Terminal 2 chạy ps -o pid,ppid,stat,cmd --ppid <pid cha>. Đổi abort() thành *(int *)0 = 1.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void describe(const char *label, int status)
{
    printf("%-22s ", label);
    if (WIFEXITED(status))
        printf("WIFEXITED   WEXITSTATUS = %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("WIFSIGNALED WTERMSIG = %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
    fflush(stdout);
}

static char proc_state(pid_t pid)
{
    char path[64], buf[256], state = '?';
    snprintf(path, sizeof path, "/proc/%d/stat", pid);
    FILE *f = fopen(path, "r");
    if (!f) return '-';
    if (fgets(buf, sizeof buf, f)) {
        char *p = strrchr(buf, ')');
        if (p && p[1] == ' ') state = p[2];
    }
    fclose(f);
    return state;
}

int main(void)
{
    int status;
    pid_t pid;

    if ((pid = fork()) == 0) exit(300);
    waitpid(pid, &status, 0);
    describe("con exit(300):", status);

    if ((pid = fork()) == 0) exit(-1);
    waitpid(pid, &status, 0);
    describe("con exit(-1):", status);

    if ((pid = fork()) == 0) abort();
    waitpid(pid, &status, 0);
    describe("con abort():", status);

    if ((pid = fork()) == 0) { pause(); _exit(0); }
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
    describe("con bi kill -9:", status);

    if ((pid = fork()) == 0) { usleep(300000); _exit(7); }
    printf("WNOHANG khi con chua xong: waitpid = %d\n", waitpid(pid, &status, WNOHANG));
    fflush(stdout);
    usleep(500000);
    printf("con da thoat, chua wait: trang thai trong /proc = %c\n", proc_state(pid));
    fflush(stdout);
    pid_t got = waitpid(pid, &status, WNOHANG);
    printf("WNOHANG lan 2: tra ve dung PID con ? %d\n", got == pid);
    describe("con exit(7):", status);
    printf("sau khi wait: /proc/<pid> = %c (khong con)\n", proc_state(pid));

    errno = 0;
    int rc = wait(&status);   /* gọi riêng: thứ tự tính các đối số của printf là không xác định */
    printf("wait() khi khong con con: %d, errno = %s\n", rc, errno == ECHILD ? "ECHILD" : strerror(errno));
    return 0;
}
