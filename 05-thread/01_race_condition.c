/*
 * Bài: Race condition với counter++
 * Chủ đề: Thread
 * Dự đoán trước khi chạy: Giá trị counter in ra có bằng 2000000 không? Chạy 3 lần có giống nhau không?
 * Ý chính: counter++ gồm 3 bước đọc – cộng – ghi, không nguyên tử. Hai thread xen kẽ nhau làm mất bớt lần tăng.
 * Thử thêm: Giảm N xuống 1000 và chạy lại vài lần — vì sao lúc này hay ra đúng?
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>

#define N 1000000
static long counter = 0;

static void *worker(void *arg)
{
    (void)arg;
    for (int i = 0; i < N; i++)
        counter++;
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    pthread_create(&t1, NULL, worker, NULL);
    pthread_create(&t2, NULL, worker, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("counter = %ld (mong doi %d)\n", counter, 2 * N);
    return 0;
}
