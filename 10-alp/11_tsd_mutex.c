/*
 * Bài: Thread-specific data (pthread_key) và các loại mutex: fast, error-checking, recursive, trylock, semaphore (ALP 4.3, 4.4)
 * Chủ đề: ALP chương 4
 * Dự đoán trước khi chạy: Mỗi thread đọc pthread_getspecific được giá trị của ai? Destructor của key chạy khi nào? Lock lần hai mutex error-checking trả về gì, mutex recursive thì sao? Thread khác unlock mutex error-checking? trylock khi mutex đang bị giữ? sem_trywait khi giá trị là 0?
 * Ý chính: Một key, mỗi thread một giá trị riêng (ví dụ file log riêng cho từng thread); destructor của key tự chạy khi thread kết thúc. Mutex mặc định (fast) lock lần hai trong cùng thread → deadlock vĩnh viễn; error-checking trả về EDEADLK; recursive đếm số lần lock và phải unlock bằng ấy lần. pthread_mutex_trylock không chặn, trả về EBUSY. Semaphore: sem_wait chặn khi 0, sem_trywait trả về -1/EAGAIN.
 * Thử thêm: Đổi kiểu mutex thứ nhất thành PTHREAD_MUTEX_NORMAL rồi lock hai lần (chương trình treo, Ctrl+C để thoát).
 */
#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static pthread_key_t thread_log_key;

static void close_thread_log(void *value)
{
    printf("  [destructor] dong \"%s\"\n", (char *)value);
    free(value);
}

static void write_to_thread_log(const char *message)
{
    printf("  %s -> ghi \"%s\"\n", (char *)pthread_getspecific(thread_log_key), message);
}

static void *thread_function(void *arg)
{
    char *name = malloc(32);
    snprintf(name, 32, "thread%ld.log", (long)arg);
    pthread_setspecific(thread_log_key, name);
    write_to_thread_log("Thread starting.");
    return NULL;
}

static const char *err(int rc)
{
    return rc == 0 ? "0" : rc == EDEADLK ? "EDEADLK" : rc == EPERM ? "EPERM" : rc == EBUSY ? "EBUSY" : strerror(rc);
}

static pthread_mutex_t errcheck;
static void *unlock_other(void *arg)
{
    (void)arg;
    printf("  thread khac unlock errorcheck: %s\n", err(pthread_mutex_unlock(&errcheck)));
    return NULL;
}

static pthread_mutex_t fast = PTHREAD_MUTEX_INITIALIZER;
static void *try_other(void *arg)
{
    (void)arg;
    printf("  thread khac trylock mutex dang bi giu: %s\n", err(pthread_mutex_trylock(&fast)));
    return NULL;
}

int main(void)
{
    printf("Thread-specific data:\n");
    pthread_key_create(&thread_log_key, close_thread_log);
    for (long i = 1; i <= 3; ++i) {
        pthread_t t;
        pthread_create(&t, NULL, thread_function, (void *)i);
        pthread_join(t, NULL);
    }
    printf("  main: pthread_getspecific = %s\n", pthread_getspecific(thread_log_key) ? "co" : "NULL");

    printf("Mutex:\n");
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&errcheck, &attr);
    printf("  errorcheck lock lan 1: %s\n", err(pthread_mutex_lock(&errcheck)));
    printf("  errorcheck lock lan 2: %s\n", err(pthread_mutex_lock(&errcheck)));
    pthread_t t;
    pthread_create(&t, NULL, unlock_other, NULL);
    pthread_join(t, NULL);
    printf("  errorcheck unlock (chu so huu): %s\n", err(pthread_mutex_unlock(&errcheck)));

    pthread_mutex_t recursive;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive, &attr);
    pthread_mutexattr_destroy(&attr);
    int r1 = pthread_mutex_lock(&recursive), r2 = pthread_mutex_lock(&recursive);
    int u1 = pthread_mutex_unlock(&recursive), u2 = pthread_mutex_unlock(&recursive), u3 = pthread_mutex_unlock(&recursive);
    printf("  recursive lock, lock: %s %s; unlock x3: %s %s %s\n", err(r1), err(r2), err(u1), err(u2), err(u3));

    pthread_mutex_lock(&fast);
    printf("  fast: trylock trong chinh thread dang giu: %s\n", err(pthread_mutex_trylock(&fast)));
    pthread_create(&t, NULL, try_other, NULL);
    pthread_join(t, NULL);
    pthread_mutex_unlock(&fast);

    printf("Semaphore:\n");
    sem_t sem;
    sem_init(&sem, 0, 2);
    int v;
    sem_getvalue(&sem, &v);
    printf("  khoi tao 2, sem_getvalue = %d\n", v);
    sem_wait(&sem);
    sem_wait(&sem);
    errno = 0;
    int rc = sem_trywait(&sem);
    printf("  sau 2 lan sem_wait: sem_trywait = %d, errno = %s\n", rc, errno == EAGAIN ? "EAGAIN" : strerror(errno));
    sem_post(&sem);
    sem_getvalue(&sem, &v);
    printf("  sau sem_post: %d\n", v);
    sem_destroy(&sem);
    return 0;
}
