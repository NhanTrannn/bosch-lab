/*
 * Bài: Integer promotion: char/short tự lên int
 * Chủ đề: C
 * Dự đoán trước khi chạy: a + b bằng bao nhiêu, còn c? sc == uc? Dòng ~x == 0xF0 in "bang" hay "khac"? sizeof(a + b)?
 * Ý chính: Trong biểu thức, char/short (kể cả unsigned) được nâng lên int TRƯỚC khi tính. Kết quả chỉ bị cắt lại khi gán về kiểu nhỏ. Bẫy kinh điển với ~ trên uint8_t trong code driver/thanh ghi.
 * Cảnh báo biên dịch cố ý: -Wsign-compare ở dòng ~x == 0xF0 (gcc nhận ra đúng cái bẫy này).
 * Thử thêm: Sửa điều kiện ~x thành (uint8_t)~x == 0xF0. Thử char c = 200; printf("%d", c); (char có dấu hay không là implementation-defined; x86 có dấu, ARM thường không dấu).
 */
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    unsigned char a = 250, b = 10;
    printf("a + b = %d\n", a + b);
    unsigned char c = a + b;
    printf("c = a + b -> %d\n", c);
    printf("sizeof(a) = %zu, sizeof(a + b) = %zu, sizeof(+a) = %zu\n", sizeof(a), sizeof(a + b), sizeof(+a));

    signed char sc = -1;
    unsigned char uc = 0xFF;
    printf("sc == uc ? %d  (sc -> %d, uc -> %d)\n", sc == uc, sc, uc);

    uint8_t x = 0x0F;
    if (~x == 0xF0)
        printf("bang\n");
    else
        printf("khac: ~x = 0x%X\n", ~x);

    uint8_t hi = 0x80;
    printf("hi << 1 = %d, (uint8_t)(hi << 1) = %d\n", hi << 1, (uint8_t)(hi << 1));

    unsigned int un = 0;
    un--;
    printf("un-- tu 0 = %u\n", un);

    short s1 = 30000, s2 = 30000;
    int sum = s1 + s2;
    short ssum = s1 + s2;
    printf("int sum = %d, short sum = %d\n", sum, ssum);
    return 0;
}
