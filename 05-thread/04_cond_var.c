/*
 * Bài: Producer–consumer một slot với condition variable
 * Chủ đề: Thread
 * Dự đoán trước khi chạy: Thứ tự các dòng produce / consume? Có thể có 2 dòng produce liền nhau không?
 * Ý chính: pthread_cond_wait nhả mutex và ngủ một cách nguyên tử; khi được đánh thức sẽ lấy lại mutex. Luôn kiểm tra điều kiện trong vòng while (spurious wakeup).
 * Thử thêm: Đổi while thành if, thêm consumer thứ 2 và dùng pthread_cond_broadcast — tìm ra kịch bản đọc slot rỗng.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
static int slot;
static int full = 0;

static void *producer(void *arg)
{
    (void)arg;
    for (int i = 1; i <= 5; i++) {
        pthread_mutex_lock(&m);
        while (full)
            pthread_cond_wait(&cv, &m);
        slot = i * 10;
        full = 1;
        printf("produce %d\n", slot);
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

static void *consumer(void *arg)
{
    (void)arg;
    for (int i = 1; i <= 5; i++) {
        pthread_mutex_lock(&m);
        while (!full)
            pthread_cond_wait(&cv, &m);
        printf("    consume %d\n", slot);
        full = 0;
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

int main(void)
{
    pthread_t p, c;
    pthread_create(&c, NULL, consumer, NULL);
    pthread_create(&p, NULL, producer, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    return 0;
}
