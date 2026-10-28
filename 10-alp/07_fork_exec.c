/*
 * Bài: system, fork + exec (họ exec: l/v/p/e), spawn như listing 3.4, nice (ALP 3.2)
 * Chủ đề: ALP chương 3
 * Dự đoán trước khi chạy: Dòng sau exec có in ra không? execvp một lệnh không tồn tại thì process con làm gì, cha thấy status gì? Biến HOME có trong chương trình chạy bằng execle không? nice(10) trong con làm priority của cha đổi không?
 * Ý chính: fork trả về 0 trong con, PID con trong cha. exec thay toàn bộ chương trình của process (PID giữ nguyên) và chỉ quay về khi LỖI. Hậu tố: l = đối số dạng list, v = mảng argv, p = tìm trong PATH, e = tự truyền môi trường. argv[0] là tên chương trình, danh sách kết thúc bằng NULL. system() chạy qua /bin/sh -c → tiện nhưng chậm và nguy hiểm với input của người dùng.
 * Thử thêm: Thay "echo" bằng "ls" với đối số "-l", "/". Chạy nice -n 10 ./build/10-alp/07_fork_exec và xem giá trị nice của cha.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

/* Listing 3.4: tạo process con chạy program. Trả về PID con. */
static pid_t spawn(char *program, char **arg_list)
{
    pid_t child_pid = fork();
    if (child_pid != 0)
        return child_pid;
    execvp(program, arg_list);
    fprintf(stderr, "  [con] execvp loi: %s\n", strerror(errno));
    abort();
}

static void wait_and_report(pid_t pid)
{
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        printf("  [cha] con %s, exit code %d\n", "thoat binh thuong", WEXITSTATUS(status));
    else
        printf("  [cha] con bi signal %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
    fflush(stdout);
}

int main(void)
{
    printf("1) system:\n");
    fflush(stdout);
    int rc = system("echo \"  [sh] \\$0 = $0\"");
    printf("  system tra ve %d\n", WEXITSTATUS(rc));

    printf("2) spawn(\"echo\", ...) bang fork + execvp:\n");
    fflush(stdout);
    char *echo_args[] = {"echo", "  [echo] xin chao tu chuong trinh moi", NULL};
    wait_and_report(spawn("echo", echo_args));

    printf("3) spawn lenh khong ton tai:\n");
    fflush(stdout);
    char *bad_args[] = {"khong_co_lenh_nay", NULL};
    wait_and_report(spawn("khong_co_lenh_nay", bad_args));

    printf("4) execl (duong dan day du) va dong sau exec:\n");
    fflush(stdout);
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/echo", "echo", "  [echo] chay bang execl", (char *)NULL);
        printf("  dong nay khong bao gio in\n");
        _exit(1);
    }
    wait_and_report(pid);

    printf("5) execle voi moi truong tu chon:\n");
    fflush(stdout);
    pid = fork();
    if (pid == 0) {
        char *env[] = {"GREETING=xin chao", NULL};
        execle("/bin/sh", "sh", "-c", "echo \"  [sh] GREETING=$GREETING, HOME=${HOME:-(khong co)}\"", (char *)NULL, env);
        _exit(1);
    }
    wait_and_report(pid);

    printf("6) nice:\n");
    fflush(stdout);
    pid = fork();
    if (pid == 0) {
        int before = getpriority(PRIO_PROCESS, 0);
        nice(10);
        printf("  [con] nice: %d -> %d\n", before, getpriority(PRIO_PROCESS, 0));
        _exit(0);
    }
    waitpid(pid, NULL, 0);
    printf("  [cha] nice van la %d\n", getpriority(PRIO_PROCESS, 0));
    return 0;
}
