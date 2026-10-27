/*
 * Bài: Mô phỏng ls / | wc -l bằng dup2
 * Chủ đề: IPC
 * Dự đoán trước khi chạy: Chương trình in ra số nào (so với chạy ls / | wc -l trong shell)? Nếu cha quên đóng fd[1] thì sao?
 * Ý chính: dup2(old, new) làm new trỏ tới cùng file với old. Shell hiện thực pipe bằng: pipe → fork 2 lần → dup2 stdout/stdin → exec.
 * Thử thêm: Comment dòng close(fd[0]); close(fd[1]); của cha. Tiếp đó mở rộng thành ls / | sort -r | head -3.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pipe(fd);

    pid_t p1 = fork();
    if (p1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("ls", "ls", "/", (char *)NULL);
        perror("exec ls");
        _exit(1);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("wc", "wc", "-l", (char *)NULL);
        perror("exec wc");
        _exit(1);
    }

    close(fd[0]);
    close(fd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    printf("Xong: tuong duong  ls / | wc -l\n");
    return 0;
}
