/*
 * !!! BÀI TREO CỐ Ý ~30 giây: nhấn Ctrl+C để thoát sớm. Cần 2 terminal.
 * Bài: Tiến trình zombie
 * Chủ đề: Process
 * Dự đoán trước khi chạy: Trong 30 giây đó, ở terminal thứ 2, cột STAT của tiến trình con hiện chữ gì? Tên lệnh của nó hiển thị thế nào?
 * Ý chính: Con đã thoát nhưng cha chưa wait() → kernel giữ lại entry trong bảng tiến trình (PID + exit status) = zombie.
 * Thử thêm: Trong 30 giây, thử kill -9 <PID con>: zombie có biến mất không? Sau đó kill cha thì sao?
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();
    if (pid == 0) {
        printf("Con %d: thoat ngay\n", getpid());
        exit(0);
    }

    printf("Cha %d: ngu 30s, KHONG goi wait(). Con = %d\n", getpid(), pid);
    printf("Terminal 2: ps -o pid,ppid,stat,cmd --ppid %d\n", getpid());
    sleep(30);
    printf("Cha thoat\n");
    return 0;
}
