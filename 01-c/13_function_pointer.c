/*
 * Bài: Con trỏ hàm, mảng con trỏ hàm và callback (qsort)
 * Chủ đề: C
 * Dự đoán trước khi chạy: Ba dòng đầu in gì? fp(1, 2) và (*fp)(1, 2) có khác nhau không? Mảng sau qsort có thứ tự nào?
 * Ý chính: Tên hàm decay thành con trỏ hàm (giống mảng). Gọi fp(x) hay (*fp)(x) như nhau. Bảng con trỏ hàm thay cho switch dài (state machine, bảng lệnh) — rất hay dùng trong embedded.
 * Thử thêm: Viết comparator sắp tăng dần. Đọc khai báo: int *(*g)(int) và int (*h[3])(void) là gì?
 */
#include <stdio.h>
#include <stdlib.h>

static int add(int a, int b) { return a + b; }
static int sub(int a, int b) { return a - b; }
static int mul(int a, int b) { return a * b; }

typedef int (*op_t)(int, int);

static int desc(const void *x, const void *y)
{
    int a = *(const int *)x, b = *(const int *)y;
    return (a < b) - (a > b); /* tránh b - a vì có thể tràn số */
}

static void apply(int *v, int n, int (*fn)(int))
{
    for (int i = 0; i < n; i++)
        v[i] = fn(v[i]);
}

static int square(int x) { return x * x; }

int main(void)
{
    op_t ops[] = {add, sub, mul};
    const char *sym = "+-*";
    for (int i = 0; i < 3; i++)
        printf("7 %c 3 = %d\n", sym[i], ops[i](7, 3));

    int (*fp)(int, int) = &add;
    printf("fp(1, 2) = %d, (*fp)(1, 2) = %d, fp == add ? %d\n", fp(1, 2), (*fp)(1, 2), fp == add);
    printf("sizeof(fp) = %zu\n", sizeof(fp));

    int v[] = {5, 1, 4, 2, 3};
    int n = sizeof(v) / sizeof(v[0]);
    qsort(v, n, sizeof(v[0]), desc);
    for (int i = 0; i < n; i++)
        printf("%d ", v[i]);
    printf("\n");

    apply(v, n, square);
    for (int i = 0; i < n; i++)
        printf("%d ", v[i]);
    printf("\n");
    return 0;
}
