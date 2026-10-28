/*
 * Lab: Sockets: socketpair, an Internet-domain socket over 127.0.0.1 (server: socket/bind/listen/accept, client: connect), byte order (ALP 5.5)
 * Topic: ALP chapter 5
 * Predict before running: Is a socketpair one-way or two-way? What is htons(80) on an x86 machine (in hex)? Which port does a server get when it binds port 0? When does read return 0?
 * Key idea: socket(domain, type, 0): domain PF_LOCAL/PF_UNIX (same machine, the address is a file path) or PF_INET (network, IP + port); type SOCK_STREAM (connected, reliable, like TCP) or SOCK_DGRAM (datagrams, like UDP). Server: socket → bind → listen → accept (returns a NEW socket for each client). Client: socket → connect. Ports and IPs must be converted to network byte order (big-endian) with htons/htonl. socketpair creates a pre-connected, two-way pair and only works with PF_LOCAL.
 * Try next: While the server waits, run ss -tlnp in Terminal 2. Change SOCK_STREAM to SOCK_DGRAM in socketpair.
 */
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    printf("htons(80) = 0x%04x, htonl(0x7f000001) = 0x%08x, ntohs(htons(8080)) = %d\n",
           htons(80), htonl(0x7f000001), ntohs(htons(8080)));

    int sv[2];
    socketpair(PF_LOCAL, SOCK_STREAM, 0, sv);
    char buf[64];
    if (fork() == 0) {
        close(sv[0]);
        ssize_t n = read(sv[1], buf, sizeof buf - 1);
        buf[n] = '\0';
        printf("  [child] got \"%s\", replying pong\n", buf);
        fflush(stdout);
        write(sv[1], "pong", 4);
        close(sv[1]);
        _exit(0);
    }
    close(sv[1]);
    write(sv[0], "ping", 4);
    ssize_t n = read(sv[0], buf, sizeof buf - 1);
    buf[n] = '\0';
    printf("  [parent] got \"%s\" on the same socket (two-way)\n", buf);
    n = read(sv[0], buf, sizeof buf);
    printf("  [parent] read after the child closed its socket = %zd (EOF)\n", n);
    close(sv[0]);
    wait(NULL);

    int server = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(0);   /* 0: the kernel picks a free port */
    bind(server, (struct sockaddr *)&addr, sizeof addr);
    listen(server, 5);
    socklen_t len = sizeof addr;
    getsockname(server, (struct sockaddr *)&addr, &len);
    int port = ntohs(addr.sin_port);
    printf("server listening on %s, port > 1023 ? %d\n", inet_ntoa(addr.sin_addr), port > 1023);
    fflush(stdout);

    if (fork() == 0) {
        close(server);
        int client = socket(PF_INET, SOCK_STREAM, 0);
        connect(client, (struct sockaddr *)&addr, sizeof addr);
        write(client, "GET /\n", 6);
        ssize_t m = read(client, buf, sizeof buf - 1);
        buf[m] = '\0';
        printf("  [client] got: %s", buf);
        close(client);
        _exit(0);
    }
    int conn = accept(server, NULL, NULL);
    n = read(conn, buf, sizeof buf - 1);
    buf[n] = '\0';
    printf("  [server] accept returned a new socket (different from the listening one ? %d), got: %s", conn != server, buf);
    fflush(stdout);
    write(conn, "HTTP/1.0 200 OK\n", 16);
    close(conn);
    wait(NULL);
    close(server);
    return 0;
}
