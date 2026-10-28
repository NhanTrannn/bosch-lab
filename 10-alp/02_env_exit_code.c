/*
 * Bài: Biến môi trường (getenv/setenv/environ) và exit code, kể cả khi bị signal giết (ALP 2.1.5, 2.1.6, 3.4)
 * Chủ đề: ALP chương 2, 3
 * Dự đoán trước khi chạy: setenv(..., 0) có ghi đè không? Process con và chương trình exec sau đó có thấy biến vừa setenv không? system("exit 300") cho WEXITSTATUS bằng mấy? Shell bị SIGTERM thì wait status là gì, và $? mà shell cha thấy là bao nhiêu?
 * Ý chính: Môi trường là mảng chuỗi "TEN=giatri" (environ), được COPY cho process con và giữ qua exec. setenv chỉ đổi môi trường của chính process (và con sinh ra sau đó), không đổi của shell cha. Exit code chỉ giữ 8 bit thấp → nên dùng 0..127; bị signal giết thì shell báo 128 + số hiệu signal.
 * Thử thêm: Trong terminal: export SERVER_NAME=abc rồi chạy lại bài; chạy chương trình rồi echo $? ngay sau đó.
 */
#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

static void show_status(const char *cmd)
{
    int status = system(cmd);
    printf("system(\"%s\"): ", cmd);
    if (WIFEXITED(status))
        printf("WIFEXITED, WEXITSTATUS = %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("WIFSIGNALED, WTERMSIG = %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
    fflush(stdout);
}

int main(void)
{
    int n = 0;
    for (char **var = environ; *var != NULL; ++var) ++n;
    printf("environ co %s bien, PATH %s\n", n > 0 ? "> 0" : "0", getenv("PATH") ? "co" : "khong co");

    char *server = getenv("SERVER_NAME");
    printf("SERVER_NAME = %s\n", server ? server : "(NULL) -> dung mac dinh server.my-company.com");

    setenv("SERVER_NAME", "backup.example.net", 1);
    setenv("SERVER_NAME", "khong-ghi-de", 0);
    printf("sau setenv: SERVER_NAME = %s\n", getenv("SERVER_NAME"));
    fflush(stdout);

    pid_t pid = fork();
    if (pid == 0) {
        printf("process con thay: %s\n", getenv("SERVER_NAME"));
        fflush(stdout);
        execlp("sh", "sh", "-c", "echo \"chuong trinh sau exec thay: $SERVER_NAME\"", (char *)NULL);
        _exit(127);
    }
    waitpid(pid, NULL, 0);

    unsetenv("SERVER_NAME");
    printf("sau unsetenv: %s\n", getenv("SERVER_NAME") ? "van con" : "NULL");
    fflush(stdout);

    show_status("exit 3");
    show_status("exit 300");
    show_status("kill -TERM $$");
    show_status("sh -c 'kill -TERM $$'; echo \"  shell thay \\$? = $?\"; exit 0");
    show_status("khong_co_lenh_nay 2>/dev/null");
    return 0;
}
