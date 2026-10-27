/*
 * Bài: fork + exec + waitpid
 * Chủ đề: Process
 * Dự đoán trước khi chạy: Mã thoát (WEXITSTATUS) mà cha in ra là bao nhiêu? Dòng perror("execlp") có chạy không?
 * Ý chính: exec thay thế toàn bộ image của tiến trình (giữ PID); thành công thì KHÔNG trả về. Cha dùng waitpid + WIFEXITED/WEXITSTATUS để lấy mã thoát.
 * Thử thêm: Đổi "/khong_ton_tai" thành "." (mã 0), rồi đổi "ls" thành "lsx" (lệnh không tồn tại).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("Con (pid %d) se chay ls\n", getpid());
        fflush(stdout);
        execlp("ls", "ls", "-l", "/khong_ton_tai", (char *)NULL);
        perror("execlp");
        exit(127);
    }

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        printf("Cha: con %d thoat voi ma %d\n", pid, WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("Cha: con %d bi signal %d\n", pid, WTERMSIG(status));
    return 0;
}
