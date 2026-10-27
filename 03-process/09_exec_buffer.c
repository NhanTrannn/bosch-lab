/*
 * Bài: exec() thay thế chương trình: PID giữ nguyên, buffer stdio mất
 * Chủ đề: Process
 * Dự đoán trước khi chạy: Dòng "chua xuong dong..." có xuất hiện không? PID sau exec có đổi không? Dòng "khong bao gio in" có in không?
 * Ý chính: exec KHÔNG tạo tiến trình mới: giữ PID, PPID, fd đang mở, nhưng xoá sạch code/data/heap/stack cũ — kể cả buffer stdio chưa flush. exec chỉ trả về khi thất bại.
 * Thử thêm: Thêm fflush(stdout) trước execlp. Đổi "sh" thành "khong_co_lenh" để thấy perror. Chạy với > out.txt: dòng PID đầu cũng mất (vì fully-buffered).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("PID truoc exec = %d\n", getpid());
    printf("chua xuong dong...");
    execlp("sh", "sh", "-c", "echo \"PID sau exec  = $$\"", (char *)NULL);
    perror("execlp");
    printf("khong bao gio in neu exec thanh cong\n");
    return 1;
}
