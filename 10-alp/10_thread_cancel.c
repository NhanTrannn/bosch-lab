/*
 * Bài: Huỷ thread: deferred cancellation, cancellation point, vùng không huỷ được, cleanup handler (ALP 4.2, 4.3.1)
 * Chủ đề: ALP chương 4
 * Dự đoán trước khi chạy: Thread A bị huỷ trong lúc sleep: cleanup handler có chạy không, join nhận được gì? Thread B tắt cancel trong lúc "chuyển tiền": bị huỷ ở đâu, dòng "chuyen xong" có in ra không? pthread_cleanup_pop(1) khi không bị huỷ thì sao?
 * Ý chính: Mặc định thread là deferred (synchronous) cancelable: yêu cầu huỷ chỉ có hiệu lực ở cancellation point (sleep, read, pthread_join, pthread_testcancel...). PTHREAD_CANCEL_DISABLE tạo vùng găng không bị huỷ; yêu cầu được giữ lại tới khi bật lại. Khi bị huỷ, cleanup handler (pthread_cleanup_push/pop) chạy để giải phóng tài nguyên. Thread bị huỷ trả về PTHREAD_CANCELED.
 * Thử thêm: Đổi thread A sang PTHREAD_CANCEL_ASYNCHRONOUS và thay sleep bằng vòng lặp bận. Bỏ pthread_testcancel ở thread B.
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void deallocate_buffer(void *buffer)
{
    printf("  [cleanup] free buffer\n");
    fflush(stdout);
    free(buffer);
}

static volatile int a_ready = 0, b_ready = 0;

static void *thread_a(void *arg)
{
    (void)arg;
    void *temp_buffer = malloc(1024);
    pthread_cleanup_push(deallocate_buffer, temp_buffer);
    a_ready = 1;
    for (int i = 0; i < 100; ++i) usleep(100000);   /* usleep là cancellation point */
    printf("  [A] khong bao gio toi day\n");
    pthread_cleanup_pop(1);
    return NULL;
}

static void *thread_b(void *arg)
{
    (void)arg;
    int old_state;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
    printf("  [B] bat dau chuyen tien (cancel bi tat)\n");
    fflush(stdout);
    b_ready = 1;
    usleep(300000);   /* là cancellation point, nhưng đang tắt cancel */
    printf("  [B] chuyen xong\n");
    fflush(stdout);
    pthread_setcancelstate(old_state, NULL);
    pthread_testcancel();
    printf("  [B] khong bao gio toi day\n");
    return NULL;
}

static void *thread_c(void *arg)
{
    (void)arg;
    void *buf = malloc(16);
    pthread_cleanup_push(deallocate_buffer, buf);
    printf("  [C] chay xong binh thuong, pop(1) van goi handler:\n");
    fflush(stdout);
    pthread_cleanup_pop(1);
    return (void *)42;
}

int main(void)
{
    pthread_t a, b, c;
    void *ret;

    printf("Thread A:\n");
    pthread_create(&a, NULL, thread_a, NULL);
    while (!a_ready) usleep(1000);
    usleep(50000);
    pthread_cancel(a);
    pthread_join(a, &ret);
    printf("  join A: %s\n", ret == PTHREAD_CANCELED ? "PTHREAD_CANCELED" : "gia tri khac");

    printf("Thread B:\n");
    fflush(stdout);
    pthread_create(&b, NULL, thread_b, NULL);
    while (!b_ready) usleep(1000);
    printf("  [main] pthread_cancel(B) = %d\n", pthread_cancel(b));
    fflush(stdout);
    pthread_join(b, &ret);
    printf("  join B: %s\n", ret == PTHREAD_CANCELED ? "PTHREAD_CANCELED" : "gia tri khac");

    printf("Thread C:\n");
    fflush(stdout);
    pthread_create(&c, NULL, thread_c, NULL);
    pthread_join(c, &ret);
    printf("  join C: %ld\n", (long)ret);
    return 0;
}
