/*
 * Bài: Truyền &i vào thread và lấy kết quả qua pthread_join
 * Chủ đề: Thread
 * Dự đoán trước khi chạy: Phần "SAI" in id nào? Phần "DUNG" in gì và tổng bằng bao nhiêu?
 * Ý chính: Mọi thread nhận CÙNG địa chỉ &i, trong khi main vẫn tiếp tục tăng i → thread đọc giá trị đã đổi (data race). Cách đúng: truyền giá trị (ép vào void*) hoặc mỗi thread một ô nhớ riêng. Giá trị trả về của thread lấy qua pthread_join.
 * Thử thêm: Chạy nhiều lần và với -fsanitize=thread. Trả về con trỏ tới biến local của thread → UB (stack đã bị huỷ).
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#define N 4

static void *bad(void *arg)
{
    printf("  SAI: thread nhan id = %d\n", *(int *)arg);
    return NULL;
}

static void *good(void *arg)
{
    intptr_t id = (intptr_t)arg;
    return (void *)(id * id);
}

int main(void)
{
    pthread_t t[N];
    for (int i = 0; i < N; i++)
        pthread_create(&t[i], NULL, bad, &i);
    for (int i = 0; i < N; i++)
        pthread_join(t[i], NULL);

    for (intptr_t i = 0; i < N; i++)
        pthread_create(&t[i], NULL, good, (void *)i);
    intptr_t sum = 0;
    for (int i = 0; i < N; i++) {
        void *ret;
        pthread_join(t[i], &ret);
        printf("  DUNG: thread %d tra ve %ld\n", i, (long)(intptr_t)ret);
        sum += (intptr_t)ret;
    }
    printf("tong = %ld\n", (long)sum);
    return 0;
}
