/*
 * Bài: Pointer arithmetic, mảng và decay
 * Chủ đề: C
 * Dự đoán trước khi chạy: sizeof(arr) trong main và sizeof(a) trong f khác nhau thế nào? *p++ in gì? pb + 1 cách pb bao nhiêu byte?
 * Ý chính: Mảng truyền vào hàm bị "decay" thành con trỏ. int *pa[3] là mảng 3 con trỏ; int (*pb)[3] là con trỏ tới mảng 3 int.
 * Thử thêm: Đổi pb thành int (*pb)[2][3] = &m; rồi in (char *)(pb + 1) - (char *)pb.
 * Cảnh báo biên dịch cố ý: -Wsizeof-array-argument ở hàm f (đó chính là điều bài muốn chỉ ra).
 */
#include <stdio.h>

void f(int a[10])
{
    printf("trong f   : sizeof(a)   = %zu\n", sizeof(a));
}

int main(void)
{
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;

    printf("trong main: sizeof(arr) = %zu\n", sizeof(arr));
    f(arr);

    printf("*(p + 2) = %d\n", *(p + 2));
    printf("p[3] = %d, 3[arr] = %d\n", p[3], 3[arr]);
    printf("*p++ = %d, ", *p++);
    printf("sau do *p = %d\n", *p);
    printf("&arr[4] - &arr[0] = %td\n", &arr[4] - &arr[0]);

    int m[2][3] = {{1, 2, 3}, {4, 5, 6}};
    int x = 7, y = 8, z = 9;
    int *pa[3] = {&x, &y, &z};
    int (*pb)[3] = m;

    printf("sizeof(pa) = %zu, sizeof(pb) = %zu\n", sizeof(pa), sizeof(pb));
    printf("*pa[1] = %d, pb[1][2] = %d\n", *pa[1], pb[1][2]);
    printf("(char *)(pb + 1) - (char *)pb = %td\n", (char *)(pb + 1) - (char *)pb);
    return 0;
}
