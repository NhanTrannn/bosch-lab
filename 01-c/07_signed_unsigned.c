/*
 * Bài: Bẫy signed/unsigned và chia nguyên
 * Chủ đề: C
 * Dự đoán trước khi chạy: Dòng A in nhánh nào? Giá trị B, C, D, E, F, G, H?
 * Ý chính: Khi so sánh/tính giữa int và unsigned int, int bị chuyển sang unsigned (usual arithmetic conversions). Chia nguyên làm tròn về 0.
 * Thử thêm: Viết vòng for (unsigned i = 5; i >= 0; i--) và dự đoán nó dừng khi nào (đừng quên Ctrl+C).
 * Cảnh báo biên dịch cố ý: -Wsign-compare ở dòng A và C (đó là bẫy cần thấy).
 */
#include <stdio.h>
#include <string.h>

int main(void)
{
    if (-1 < 1U)
        printf("A: -1 < 1U\n");
    else
        printf("A: -1 >= 1U\n");

    unsigned int u = 3;
    printf("B: u - 5 = %u\n", u - 5);

    int i = -1;
    unsigned int k = 1;
    printf("C: i > k ? %d\n", i > k);

    printf("D: 7/2=%d  -7/2=%d  -7%%2=%d  7%%-2=%d  7/2.0=%.1f\n",
           7 / 2, -7 / 2, -7 % 2, 7 % -2, 7 / 2.0);

    char s[] = "abc";
    printf("E: sizeof(\"abc\")=%zu sizeof(s)=%zu strlen(s)=%zu\n",
           sizeof("abc"), sizeof(s), strlen(s));

    unsigned char c = 255;
    c++;
    printf("F: c = %d\n", c);

    short sh = -1;
    printf("G: (unsigned short)sh = %u\n", (unsigned short)sh);

    printf("H: int=%zu long=%zu ptr=%zu char=%zu\n",
           sizeof(int), sizeof(long), sizeof(void *), sizeof(char));
    return 0;
}
