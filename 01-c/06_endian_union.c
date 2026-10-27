/*
 * Bài: Endianness bằng pointer và union
 * Chủ đề: C
 * Dự đoán trước khi chạy: Trên PC x86_64, byte đầu tiên của 0x12345678 trong bộ nhớ là gì? Dòng "Cach 2" in 4 byte theo thứ tự nào?
 * Ý chính: Little endian lưu byte thấp (LSB) ở địa chỉ thấp. Các member của union dùng chung một vùng nhớ.
 * Thử thêm: Viết hàm swap32 đổi endian bằng dịch bit, so với htonl() trong <arpa/inet.h>.
 */
#include <stdio.h>
#include <stdint.h>

union U {
    uint32_t value;
    uint8_t bytes[4];
};

int main(void)
{
    uint32_t x = 0x12345678;
    uint8_t *p = (uint8_t *)&x;
    printf("Cach 1: byte dau = 0x%02X -> %s endian\n",
           p[0], p[0] == 0x78 ? "little" : "big");

    union U u;
    u.value = 0x12345678;
    printf("Cach 2: ");
    for (int i = 0; i < 4; i++)
        printf("%02X ", u.bytes[i]);
    printf("\n");

    union { int i; char c; } t = { .i = 1 };
    printf("Cach 3: t.c = %d\n", t.c);
    printf("sizeof(union U) = %zu\n", sizeof(union U));
    return 0;
}
