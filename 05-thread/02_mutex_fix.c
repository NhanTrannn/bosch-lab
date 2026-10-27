/*
 * Bài: Sửa race bằng pthread_mutex
 * Chủ đề: Thread
 * Dự đoán trước khi chạy: counter giờ bằng bao nhiêu? Chạy chậm hơn hay nhanh hơn bài 01?
 * Ý chính: Mutex đảm bảo tại một thời điểm chỉ một thread vào vùng găng (critical section). Đổi lại là chi phí lock/unlock.
 * Thử thêm: Đo thời gian bằng time ./build/05-thread/02_mutex_fix; sau đó chuyển lock ra ngoài vòng for và đo lại.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>

#define N 1000000
static long counter = 0;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void *worker(void *arg)
{
    (void)arg;
    for (int i = 0; i < N; i++) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
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
    pthread_mutex_destroy(&lock);
    return 0;
}
