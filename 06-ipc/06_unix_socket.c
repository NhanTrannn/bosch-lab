/*
 * Bài: UNIX domain socket: server/client bằng fork
 * Chủ đề: IPC
 * Dự đoán trước khi chạy: Hai dòng in ra theo thứ tự nào? Vì sao bind/listen phải làm TRƯỚC fork?
 * Ý chính: Server: socket → bind → listen → accept. Client: socket → connect. AF_UNIX dùng đường dẫn file làm địa chỉ, chỉ trong 1 máy.
 * Thử thêm: Chạy lại ngay mà bỏ dòng unlink(SOCK_PATH) ở đầu main: bind báo lỗi gì?
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#define SOCK_PATH "/tmp/bosch.sock"

int main(void)
{
    unlink(SOCK_PATH);
    int srv = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);
    if (bind(srv, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }
    listen(srv, 1);

    pid_t pid = fork();
    if (pid == 0) {
        close(srv);
        int c = socket(AF_UNIX, SOCK_STREAM, 0);
        connect(c, (struct sockaddr *)&addr, sizeof(addr));
        write(c, "ping", 4);
        char buf[64] = {0};
        read(c, buf, sizeof(buf) - 1);
        printf("Client nhan: %s\n", buf);
        close(c);
        return 0;
    }

    int conn = accept(srv, NULL, NULL);
    char buf[64] = {0};
    read(conn, buf, sizeof(buf) - 1);
    printf("Server nhan: %s\n", buf);
    fflush(stdout);
    write(conn, "pong", 4);
    close(conn);
    close(srv);
    waitpid(pid, NULL, 0);
    unlink(SOCK_PATH);
    return 0;
}
