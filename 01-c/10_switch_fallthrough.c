/*
 * Bài: switch thiếu break
 * Chủ đề: C
 * Dự đoán trước khi chạy: Với x từ 0 đến 4, mỗi dòng in gì?
 * Ý chính: Không có break thì chạy tiếp xuống các case bên dưới (fall-through). default không cần đứng cuối.
 * Thử thêm: Thêm break sau "three " và đoán lại dòng x = 3.
 * Cảnh báo biên dịch cố ý: -Wimplicit-fallthrough (cố ý, để thấy compiler cảnh báo chỗ nào).
 */
#include <stdio.h>

void test(int x)
{
    printf("x = %d: ", x);
    switch (x) {
    case 1:
        printf("one ");
    case 2:
        printf("two ");
        break;
    case 3:
        printf("three ");
    default:
        printf("default ");
    case 4:
        printf("four ");
    }
    printf("\n");
}

int main(void)
{
    for (int i = 0; i <= 4; i++)
        test(i);
    return 0;
}
