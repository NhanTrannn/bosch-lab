/*
 * Lab: UNIX domain socket: a server/client pair using fork
 * Topic: IPC
 * Predict before running: In what order are the two lines printed? Why must bind/listen happen BEFORE fork?
 * Key idea: Server: socket → bind → listen → accept. Client: socket → connect. AF_UNIX uses a file path as the address and only works within one machine.
 * Try next: Run it again right away without the unlink(SOCK_PATH) at the start of main: what error does bind report?
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
        printf("Client got: %s\n", buf);
        close(c);
        return 0;
    }

    int conn = accept(srv, NULL, NULL);
    char buf[64] = {0};
    read(conn, buf, sizeof(buf) - 1);
    printf("Server got: %s\n", buf);
    fflush(stdout);
    write(conn, "pong", 4);
    close(conn);
    close(srv);
    waitpid(pid, NULL, 0);
    unlink(SOCK_PATH);
    return 0;
}
