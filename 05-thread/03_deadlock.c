/*
 * !!! BÀI TREO CỐ Ý: chương trình sẽ đứng yên, nhấn Ctrl+C để thoát.
 * Bài: Deadlock do khoá ngược thứ tự
 * Chủ đề: Thread
 * Dự đoán trước khi chạy: Chương trình in những dòng nào rồi sao nữa? Dòng "Khong deadlock" có xuất hiện không?
 * Ý chính: T1 giữ A chờ B, T2 giữ B chờ A → chờ vòng tròn mãi mãi. Cách sửa phổ biến nhất: mọi thread khoá các mutex theo CÙNG một thứ tự.
 * Thử thêm: Sửa T2 khoá A rồi mới B. Hoặc dùng pthread_mutex_trylock và nhả A nếu không lấy được B.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t A = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t B = PTHREAD_MUTEX_INITIALIZER;

static void *t1(void *arg)
{
    (void)arg;
    pthread_mutex_lock(&A);
    printf("T1: giu A, cho B...\n");
    usleep(100000);
    pthread_mutex_lock(&B);
    printf("T1: co ca A va B\n");
    pthread_mutex_unlock(&B);
    pthread_mutex_unlock(&A);
    return NULL;
}

static void *t2(void *arg)
{
    (void)arg;
    pthread_mutex_lock(&B);
    printf("T2: giu B, cho A...\n");
    usleep(100000);
    pthread_mutex_lock(&A);
    printf("T2: co ca A va B\n");
    pthread_mutex_unlock(&A);
    pthread_mutex_unlock(&B);
    return NULL;
}

int main(void)
{
    pthread_t x, y;
    pthread_create(&x, NULL, t1, NULL);
    pthread_create(&y, NULL, t2, NULL);
    pthread_join(x, NULL);
    pthread_join(y, NULL);
    printf("Khong deadlock!\n");
    return 0;
}
