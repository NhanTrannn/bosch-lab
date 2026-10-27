/*
 * Bài: Chuỗi: mảng char và con trỏ tới string literal
 * Chủ đề: C
 * Dự đoán trước khi chạy: sizeof và strlen của a, b, p khác nhau thế nào? sizeof("ab\0cd") và strlen của nó? sizeof('A') trong C bằng mấy? d in ra gì?
 * Ý chính: char a[] = "..." là MẢNG (bản sao sửa được), const char *p = "..." là CON TRỎ tới literal trong vùng chỉ đọc (.rodata). sizeof đo kiểu, strlen đếm tới '\0'.
 * Cảnh báo biên dịch cố ý: -Wsizeof-array-argument ở hàm f (sizeof của tham số mảng).
 * Thử thêm: Viết ((char *)p)[0] = 'B'; → Segmentation fault (literal nằm ở vùng chỉ đọc). Đổi strncpy thành strcpy với chuỗi dài → tràn bộ đệm (thử với -fsanitize=address).
 */
#include <stdio.h>
#include <string.h>

void f(char s[])
{
    printf("trong f: sizeof(s) = %zu, strlen(s) = %zu\n", sizeof(s), strlen(s));
}

int main(void)
{
    char a[] = "bosch";
    char b[10] = "bosch";
    const char *p = "bosch";
    char c[5] = "bosch"; /* vừa đủ 5 ký tự, KHÔNG còn chỗ cho '\0' (C cho phép, C++ báo lỗi) */

    printf("a: sizeof = %zu, strlen = %zu\n", sizeof(a), strlen(a));
    printf("b: sizeof = %zu, strlen = %zu\n", sizeof(b), strlen(b));
    printf("p: sizeof = %zu, strlen = %zu\n", sizeof(p), strlen(p));
    printf("c: sizeof = %zu (strlen(c) la UB vi khong co '\\0')\n", sizeof(c));
    printf("\"ab\\0cd\": sizeof = %zu, strlen = %zu\n", sizeof("ab\0cd"), strlen("ab\0cd"));
    printf("sizeof('A') = %zu, sizeof(char) = %zu\n", sizeof('A'), sizeof(char));
    f(a);

    a[0] = 'B';
    printf("a sau khi sua = %s\n", a);
    /* p[0] = 'B'; */ /* lỗi biên dịch vì const; ép bỏ const thì crash: literal nằm ở vùng chỉ đọc */

    char d[8];
    strncpy(d, "abcdefghij", sizeof(d)); /* strncpy KHÔNG tự thêm '\0' khi nguồn dài hơn */
    d[sizeof(d) - 1] = '\0';
    printf("d = %s\n", d);

    printf("strcmp: %d %d %d\n", strcmp("abc", "abd") < 0, strcmp("b", "abc") > 0, strcmp("ab", "ab") == 0);
    printf("a == \"Bosch\" ? %d, strcmp = %d\n", a == (char *)"Bosch", strcmp(a, "Bosch"));
    return 0;
}
