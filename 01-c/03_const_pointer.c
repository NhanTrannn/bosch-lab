/*
 * Bài: Ba kiểu const với con trỏ
 * Chủ đề: C
 * Dự đoán trước khi chạy: Chương trình in gì? Lần lượt bỏ comment từng dòng đang bị comment trong main: dòng nào biên dịch được, dòng nào lỗi và lỗi gì?
 * Ý chính: Đọc từ phải sang trái: const đứng trước * khoá dữ liệu được trỏ tới, const đứng sau * khoá chính con trỏ.
 * Thử thêm: Thử gán int *q = p1; (bỏ const) và xem compiler cảnh báo gì.
 */
#include <stdio.h>

int main(void)
{
    int a = 1, b = 2;

    const int *p1 = &a;          /* (1) */
    int *const p2 = &a;          /* (2) */
    const int *const p3 = &a;    /* (3) */

    p1 = &b;
    /* *p1 = 10; */

    *p2 = 10;
    /* p2 = &b; */

    /* *p3 = 20; */
    /* p3 = &b; */

    printf("a = %d, b = %d, *p1 = %d, *p2 = %d, *p3 = %d\n",
           a, b, *p1, *p2, *p3);
    return 0;
}
