/*
 * Bài: static local, static mức file và extern
 * Chủ đề: C
 * Dự đoán trước khi chạy: n và m in ra gì sau 3 lần gọi counter()? Nếu bỏ comment dòng extern int g_hidden và dùng nó thì lỗi ở bước compile hay link?
 * Ý chính: static local sống suốt chương trình, chỉ khởi tạo 1 lần. static ở mức file = internal linkage (file khác không thấy). extern chỉ khai báo, định nghĩa ở file khác.
 * Thử thêm: Bỏ chữ static trước g_hidden trong 04_helper.c rồi build lại phần extern.
 */
#include <stdio.h>

extern int g_shared;             /* định nghĩa ở 04_helper.c */
int helper_get_hidden(void);
/* extern int g_hidden; */       /* bỏ comment và in g_hidden trong main */

static int file_counter = 0;

int counter(void)
{
    static int n = 0;
    int m = 0;
    n++;
    m++;
    file_counter++;
    printf("n = %d, m = %d\n", n, m);
    return n;
}

int main(void)
{
    for (int i = 0; i < 3; i++)
        counter();
    printf("file_counter = %d\n", file_counter);
    g_shared += 1;
    printf("g_shared = %d, hidden = %d\n", g_shared, helper_get_hidden());
    return 0;
}
