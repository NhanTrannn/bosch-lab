/*
 * Bài: pthread: truyền dữ liệu vào thread, giá trị trả về qua join, pthread_exit, thread detached (ALP 4.1)
 * Chủ đề: ALP chương 4
 * Dự đoán trước khi chạy: Số nguyên tố thứ 1000 mà thread tính được là bao nhiêu? pthread_equal(self, main) trong thread con? Gọi pthread_join lên thread detached thì sao? Hai thread in ký tự dùng chung struct trên stack của main có an toàn không?
 * Ý chính: pthread_create(&id, attr, hàm, void *arg): chỉ truyền được MỘT con trỏ → gói nhiều tham số vào struct. Vùng nhớ arg phải sống tới khi thread dùng xong (vì vậy main phải join trước khi return). Giá trị trả về là void*, lấy bằng pthread_join (return hoặc pthread_exit đều được). Thread detached tự giải phóng khi kết thúc và không join được.
 * Thử thêm: Bỏ các pthread_join của char_print rồi chạy vài lần. Biên dịch không có -pthread để xem lỗi link.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Listing 4.2, main đã sửa như listing 4.3 */
struct char_print_parms { char character; int count; };

static void *char_print(void *parameters)
{
    struct char_print_parms *p = parameters;
    for (int i = 0; i < p->count; ++i) fputc(p->character, stderr);
    return NULL;
}

/* Listing 4.4: tính số nguyên tố thứ n (chậm), trả về qua void* */
static void *compute_prime(void *arg)
{
    int candidate = 2;
    int n = *((int *)arg);
    while (1) {
        int factor, is_prime = 1;
        for (factor = 2; factor < candidate; ++factor)
            if (candidate % factor == 0) { is_prime = 0; break; }
        if (is_prime && --n == 0)
            return (void *)(intptr_t)candidate;
        ++candidate;
    }
}

static pthread_t main_thread;
static void *who_am_i(void *arg)
{
    (void)arg;
    printf("trong thread con: pthread_equal(self, main) = %d\n", pthread_equal(pthread_self(), main_thread) != 0);
    pthread_exit((void *)(intptr_t)7);
}

static sem_t release;
static void *detached_worker(void *arg)
{
    (void)arg;
    sem_wait(&release);
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    struct char_print_parms a = {'x', 30}, b = {'o', 20};
    pthread_create(&t1, NULL, char_print, &a);
    pthread_create(&t2, NULL, char_print, &b);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    fprintf(stderr, "\n(30 x va 20 o, thu tu xen ke khong co dinh)\n");

    int which = 1000;
    void *prime;
    pthread_t pt;
    pthread_create(&pt, NULL, compute_prime, &which);
    pthread_join(pt, &prime);
    printf("so nguyen to thu %d = %d\n", which, (int)(intptr_t)prime);

    main_thread = pthread_self();
    void *ret;
    pthread_t w;
    pthread_create(&w, NULL, who_am_i, NULL);
    pthread_join(w, &ret);
    printf("pthread_exit((void*)7) -> join nhan %d\n", (int)(intptr_t)ret);

    sem_init(&release, 0, 0);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t d;
    pthread_create(&d, &attr, detached_worker, NULL);
    pthread_attr_destroy(&attr);
    int rc = pthread_join(d, NULL);
    printf("pthread_join thread detached: %d (%s)\n", rc, rc == EINVAL ? "EINVAL" : strerror(rc));
    sem_post(&release);
    return 0;
}
