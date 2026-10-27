/*
 * Bài: pipe: cha ghi, con đọc
 * Chủ đề: IPC
 * Dự đoán trước khi chạy: Con đọc được bao nhiêu byte? read trả về bao nhiêu ở lần cuối? Nếu con KHÔNG đóng fd[1] thì sao?
 * Ý chính: pipe(fd): fd[0] để đọc, fd[1] để ghi. read trả 0 (EOF) chỉ khi MỌI đầu ghi đã đóng — kể cả đầu ghi mà chính tiến trình đọc đang giữ.
 * Thử thêm: Comment dòng close(fd[1]) trong nhánh con và chạy lại (Ctrl+C để thoát).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(fd[1]);
        char buf[64];
        ssize_t n;
        while ((n = read(fd[0], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            printf("Con doc %zd byte: %s\n", n, buf);
        }
        printf("Con: read tra ve %zd\n", n);
        close(fd[0]);
        return 0;
    }

    close(fd[0]);
    const char *msg = "xin chao tu cha";
    write(fd[1], msg, strlen(msg));
    close(fd[1]);
    waitpid(pid, NULL, 0);
    printf("Cha: xong\n");
    return 0;
}
