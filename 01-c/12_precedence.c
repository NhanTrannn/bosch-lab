/*
 * Bài: Độ ưu tiên toán tử và short-circuit
 * Chủ đề: C
 * Dự đoán trước khi chạy: Giá trị từng dòng? Sau dòng || và && thì i, k bằng bao nhiêu? arr[1] cuối cùng bằng mấy?
 * Ý chính: == và != ưu tiên CAO hơn & | ^; + - ưu tiên cao hơn << >>; ?: kết hợp từ phải. && và || dừng sớm (short-circuit) và có sequence point.
 * Cảnh báo biên dịch cố ý: -Wparentheses (a & 4 == 4, 1 << 2 + 1) và -Wunused-value (toán tử phẩy).
 * Thử thêm: Viết lại mọi dòng có cảnh báo bằng dấu ngoặc cho đúng ý định. Thử in i++ + ++i (UB — đừng viết thế trong code thật).
 */
#include <stdio.h>

int main(void)
{
    int a = 6; /* 110b: bit 2 bật */
    printf("a & 4 == 4   -> %d\n", a & 4 == 4);
    printf("(a & 4) == 4 -> %d\n", (a & 4) == 4);
    printf("1 << 2 + 1   -> %d\n", 1 << 2 + 1);
    printf("10 - 2 - 3   -> %d\n", 10 - 2 - 3);
    printf("2 * 3 %% 4    -> %d\n", 2 * 3 % 4);

    int arr[] = {10, 20, 30};
    int *p = arr;
    int v = *p++;  /* *(p++) */
    int w = ++*p;  /* ++(*p): tăng arr[1] */
    int y = (*p)++;
    printf("v = %d, w = %d, y = %d, arr[1] = %d\n", v, w, y, arr[1]);

    int q = 5;
    int r = q > 3 ? q < 10 ? 1 : 2 : 3;
    printf("?: long nhau -> %d\n", r);

    int comma = (1, 2, 3);
    printf("(1, 2, 3)    -> %d\n", comma);

    int i = 0;
    int j = i++ || i++;
    printf("j = %d, i = %d\n", j, i);

    int k = 0;
    int m = k++ && k++;
    printf("m = %d, k = %d\n", m, k);

    printf("!5 = %d, !!5 = %d, ~5 = %d, -5 >> 1 = %d\n", !5, !!5, ~5, -5 >> 1);
    return 0;
}
