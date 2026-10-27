/*
 * Bài: Phạm vi biến (scope), che tên (shadowing) và static trong vòng lặp
 * Chủ đề: C
 * Dự đoán trước khi chạy: Mỗi dòng x= in số mấy? Ba lần gọi f() in gì? Vòng for cuối in s, t thế nào?
 * Ý chính: Biến trong khối trong che biến cùng tên ở ngoài. static local chỉ khởi tạo MỘT lần và sống suốt chương trình; biến auto khởi tạo lại mỗi lần vào khối. Biến static/global không khởi tạo = 0 (nằm ở .bss).
 * Thử thêm: Biên dịch với -Wshadow để thấy cảnh báo. Bỏ "= 0" ở int m → giá trị rác (UB), thử với valgrind.
 */
#include <stdio.h>

int x = 1;

void f(void)
{
    static int n;   /* = 0 */
    int m = 0;
    n++;
    m++;
    printf("f: n = %d, m = %d\n", n, m);
}

int main(void)
{
    printf("x = %d\n", x);
    int x = 2;
    {
        int x = 3;
        printf("x = %d (khoi trong)\n", x);
        {
            extern int x; /* lấy lại biến toàn cục */
            printf("x = %d (extern)\n", x);
        }
    }
    printf("x = %d\n", x);

    for (int i = 0; i < 3; i++)
        f();

    for (int i = 0; i < 3; i++) {
        static int s = 10;
        int t = 10;
        s++;
        t++;
        printf("s = %d, t = %d\n", s, t);
    }
    return 0;
}
