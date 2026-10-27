/*
 * Bài: Semaphore giới hạn 2 thread cùng lúc
 * Chủ đề: Thread
 * Dự đoán trước khi chạy: Giá trị "toi da cung luc" là bao nhiêu? Thứ tự vào/ra có cố định không?
 * Ý chính: Counting semaphore khởi tạo = 2: sem_wait giảm (chặn nếu đang 0), sem_post tăng. Dùng để giới hạn số thread dùng một tài nguyên.
 * Thử thêm: Khởi tạo sem_init(&sem, 0, 1) (binary semaphore) và so sánh với mutex.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

static sem_t sem;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static int inside = 0, max_inside = 0;

static void *worker(void *arg)
{
    int id = (int)(long)arg;
    sem_wait(&sem);

    pthread_mutex_lock(&m);
    inside++;
    if (inside > max_inside)
        max_inside = inside;
    printf("T%d vao (dang trong: %d)\n", id, inside);
    pthread_mutex_unlock(&m);

    usleep(200000);

    pthread_mutex_lock(&m);
    inside--;
    printf("T%d ra\n", id);
    pthread_mutex_unlock(&m);

    sem_post(&sem);
    return NULL;
}

int main(void)
{
    pthread_t t[5];
    sem_init(&sem, 0, 2);
    for (long i = 0; i < 5; i++)
        pthread_create(&t[i], NULL, worker, (void *)i);
    for (int i = 0; i < 5; i++)
        pthread_join(t[i], NULL);
    printf("So thread toi da cung luc = %d\n", max_inside);
    sem_destroy(&sem);
    return 0;
}
