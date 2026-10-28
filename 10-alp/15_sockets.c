/*
 * Bài: Socket: socketpair, Internet-domain socket qua 127.0.0.1 (server: socket/bind/listen/accept, client: connect), byte order (ALP 5.5)
 * Chủ đề: ALP chương 5
 * Dự đoán trước khi chạy: socketpair cho giao tiếp một chiều hay hai chiều? htons(80) trên máy x86 bằng bao nhiêu (hex)? Server bind port 0 thì được port nào? read trả về 0 khi nào?
 * Ý chính: socket(domain, type, 0): domain PF_LOCAL/PF_UNIX (cùng máy, địa chỉ là đường dẫn file) hoặc PF_INET (mạng, IP + port); type SOCK_STREAM (kết nối, tin cậy, như TCP) hoặc SOCK_DGRAM (gói tin, như UDP). Server: socket → bind → listen → accept (trả về socket MỚI cho từng client). Client: socket → connect. Port và IP phải đổi sang network byte order (big-endian) bằng htons/htonl. socketpair tạo cặp socket nối sẵn, hai chiều, chỉ dùng được với PF_LOCAL.
 * Thử thêm: Trong lúc server chờ, ở Terminal 2 chạy ss -tlnp. Đổi SOCK_STREAM thành SOCK_DGRAM với socketpair.
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
        printf("  [con] nhan \"%s\", tra loi pong\n", buf);
        fflush(stdout);
        write(sv[1], "pong", 4);
        close(sv[1]);
        _exit(0);
    }
    close(sv[1]);
    write(sv[0], "ping", 4);
    ssize_t n = read(sv[0], buf, sizeof buf - 1);
    buf[n] = '\0';
    printf("  [cha] nhan \"%s\" qua cung socket (hai chieu)\n", buf);
    n = read(sv[0], buf, sizeof buf);
    printf("  [cha] read sau khi con dong socket = %zd (EOF)\n", n);
    close(sv[0]);
    wait(NULL);

    int server = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(0);   /* 0: kernel tự chọn port trống */
    bind(server, (struct sockaddr *)&addr, sizeof addr);
    listen(server, 5);
    socklen_t len = sizeof addr;
    getsockname(server, (struct sockaddr *)&addr, &len);
    int port = ntohs(addr.sin_port);
    printf("server lang nghe tai %s, port > 1023 ? %d\n", inet_ntoa(addr.sin_addr), port > 1023);
    fflush(stdout);

    if (fork() == 0) {
        close(server);
        int client = socket(PF_INET, SOCK_STREAM, 0);
        connect(client, (struct sockaddr *)&addr, sizeof addr);
        write(client, "GET /\n", 6);
        ssize_t m = read(client, buf, sizeof buf - 1);
        buf[m] = '\0';
        printf("  [client] nhan: %s", buf);
        close(client);
        _exit(0);
    }
    int conn = accept(server, NULL, NULL);
    n = read(conn, buf, sizeof buf - 1);
    buf[n] = '\0';
    printf("  [server] accept tra ve socket moi (khac socket lang nghe ? %d), nhan: %s", conn != server, buf);
    fflush(stdout);
    write(conn, "HTTP/1.0 200 OK\n", 16);
    close(conn);
    wait(NULL);
    close(server);
    return 0;
}
