/*
 * Bài: Memory layout: text, data, bss, heap, stack
 * Chủ đề: C
 * Dự đoán trước khi chạy: Mỗi biến nằm ở vùng nhớ nào (text / rodata / data / bss / heap / stack)? Sắp xếp địa chỉ từ thấp đến cao. g_uninit và s_global bằng bao nhiêu?
 * Ý chính: Code ở text, hằng ở rodata, global/static có khởi tạo ở data, chưa khởi tạo ở bss (tự = 0), malloc ở heap (lớn dần lên), biến cục bộ ở stack (gần đỉnh, lớn dần xuống).
 * Thử thêm: Chạy 2 lần liền: địa chỉ có đổi không? Chạy size ./build/01-c/09_memory_layout và cat /proc/self/maps.
 */
#include <stdio.h>
#include <stdlib.h>

int g_init = 10;
int g_uninit;
static int s_global;
const int g_const = 5;

int main(void)
{
    static int s_local = 1;
    int local = 2;
    int *heap = malloc(sizeof(int));
    const char *str = "hello";

    printf("main      : %p\n", (void *)main);
    printf("\"hello\"   : %p\n", (void *)str);
    printf("g_const   : %p\n", (void *)&g_const);
    printf("g_init    : %p\n", (void *)&g_init);
    printf("s_local   : %p\n", (void *)&s_local);
    printf("g_uninit  : %p\n", (void *)&g_uninit);
    printf("s_global  : %p\n", (void *)&s_global);
    printf("heap      : %p\n", (void *)heap);
    printf("local     : %p\n", (void *)&local);

    printf("g_uninit = %d, s_global = %d\n", g_uninit, s_global);
    free(heap);
    return 0;
}
