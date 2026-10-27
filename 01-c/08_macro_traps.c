/*
 * Bài: Bẫy macro
 * Chủ đề: C
 * Dự đoán trước khi chạy: Giá trị của từng dòng? Sau SQ_OK(i++) và MAX(j++, 3) thì i, j bằng bao nhiêu?
 * Ý chính: Macro chỉ thay thế văn bản. Luôn bọc ngoặc từng tham số và cả biểu thức; tham số có side effect có thể bị tính nhiều lần.
 * Thử thêm: Viết lại SQ và MAX thành static inline function, chạy lại và so sánh i, j.
 * Cảnh báo biên dịch cố ý: -Wsequence-point ở dòng SQ_OK(i++) (undefined behavior cố ý).
 */
#include <stdio.h>

#define SQ(x)      x * x
#define SQ_OK(x)   ((x) * (x))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define DOUBLE(x)  x + x

int main(void)
{
    int a = 3;
    printf("SQ(a + 1)     = %d\n", SQ(a + 1));
    printf("SQ_OK(a + 1)  = %d\n", SQ_OK(a + 1));
    printf("10 / SQ(2)    = %d\n", 10 / SQ(2));
    printf("10 / SQ_OK(2) = %d\n", 10 / SQ_OK(2));
    printf("DOUBLE(5) * 2 = %d\n", DOUBLE(5) * 2);

    int i = 5;
    int r = SQ_OK(i++);
    printf("SQ_OK(i++) = %d, i = %d\n", r, i);

    int j = 5;
    int m = MAX(j++, 3);
    printf("MAX(j++, 3) = %d, j = %d\n", m, j);
    return 0;
}
