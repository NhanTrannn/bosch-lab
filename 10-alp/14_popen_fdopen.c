/*
 * Bài: Pipe với FILE*: fdopen (listing 5.7), dup2 + exec sort (listing 5.8), popen/pclose (listing 5.9)
 * Chủ đề: ALP chương 5
 * Dự đoán trước khi chạy: Các dòng gửi cho sort hiện ra lúc nào và theo thứ tự nào? popen("...; exit 3", "r") thì pclose trả về status gì? Process con đọc pipe bằng fgets dừng khi nào?
 * Ý chính: pipe(fds): fds[0] đầu đọc, fds[1] đầu ghi. fdopen bọc fd thành FILE* để dùng fprintf/fgets. Phải đóng đầu không dùng, nếu không bên đọc không bao giờ thấy EOF. dup2(fds[0], STDIN_FILENO) rồi exec → chương trình khác đọc từ pipe như stdin. popen gói gọn pipe + fork + exec sh -c; pclose chờ con và trả về wait status.
 * Thử thêm: Bỏ close(fds[1]) trong process con chạy sort → sort treo vì không bao giờ thấy EOF. popen("ls -l /", "r") và đọc từng dòng.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int fds[2];
    pid_t pid;

    printf("1) pipe + fdopen:\n");
    fflush(stdout);
    pipe(fds);
    pid = fork();
    if (pid == 0) {
        close(fds[1]);
        FILE *stream = fdopen(fds[0], "r");
        char buffer[64];
        while (fgets(buffer, sizeof buffer, stream) != NULL)
            printf("  [con] nhan: %s", buffer);
        printf("  [con] fgets tra ve NULL: EOF vi cha da dong dau ghi\n");
        fclose(stream);
        _exit(0);
    }
    close(fds[0]);
    FILE *stream = fdopen(fds[1], "w");
    for (int i = 1; i <= 3; ++i) fprintf(stream, "Hello, world #%d\n", i);
    fclose(stream);
    waitpid(pid, NULL, 0);

    printf("2) dup2 + execlp(\"sort\"):\n");
    fflush(stdout);
    pipe(fds);
    pid = fork();
    if (pid == 0) {
        close(fds[1]);
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);
        execlp("sort", "sort", (char *)NULL);
        _exit(127);
    }
    close(fds[0]);
    stream = fdopen(fds[1], "w");
    fprintf(stream, "  This is a test.\n  Hello, world.\n  My dog has fleas.\n  This program is great.\n  One fish, two fish.\n");
    fflush(stream);
    printf("  [cha] da ghi xong 5 dong, sort chua in gi vi chua thay EOF\n");
    fflush(stdout);
    close(fds[1]);
    waitpid(pid, NULL, 0);

    printf("3) popen(\"sort\", \"w\"):\n");
    fflush(stdout);
    FILE *out = popen("sort", "w");
    fprintf(out, "  chuoi\n  tao\n  cam\n");
    int status = pclose(out);
    printf("  pclose = %d (WEXITSTATUS %d)\n", status, WEXITSTATUS(status));

    printf("4) popen(..., \"r\"):\n");
    FILE *in = popen("echo $((6 * 7)); exit 3", "r");
    char line[32];
    fgets(line, sizeof line, in);
    line[strcspn(line, "\n")] = '\0';
    status = pclose(in);
    printf("  doc duoc \"%s\", pclose -> WEXITSTATUS %d\n", line, WEXITSTATUS(status));
    return 0;
}
