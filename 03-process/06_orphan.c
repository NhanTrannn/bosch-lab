/*
 * Bài: Tiến trình mồ côi (orphan)
 * Chủ đề: Process
 * Dự đoán trước khi chạy: PPID mà con in ra ở dòng "truoc" và dòng "sau" khác nhau thế nào? Dấu nhắc shell xuất hiện ở đâu?
 * Ý chính: Khi cha thoát trước, con được tiến trình init (PID 1) hoặc một "subreaper" (vd systemd --user) nhận nuôi.
 * Thử thêm: Chạy ps -o pid,cmd -p <PPID mới> để xem ai đã nhận nuôi tiến trình con.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();
    if (pid > 0) {
        printf("Cha %d: thoat sau 1s\n", getpid());
        sleep(1);
        return 0;
    }

    printf("Con %d: truoc, PPID = %d\n", getpid(), getppid());
    sleep(3);
    printf("Con %d: sau,   PPID = %d\n", getpid(), getppid());
    return 0;
}
