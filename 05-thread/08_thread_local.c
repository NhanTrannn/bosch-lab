/*
 * Bài: Thread chia sẻ gì, riêng gì: global, thread-local, PID/TID
 * Chủ đề: Thread
 * Dự đoán trước khi chạy: Mỗi thread in mine và shared bằng mấy? Cuối cùng main thấy mine, shared bằng mấy? PID của thread có bằng PID của main không?
 * Ý chính: Các thread trong một tiến trình dùng chung heap, global, fd, signal handler, PID; mỗi thread có stack, thanh ghi, TID và biến _Thread_local riêng. Trên Linux thread là task có TID riêng (ps -L), getpid() trả về TGID chung.
 * Thử thêm: Chạy và xem ps -eLf | grep 08_thread trong lúc chạy (thêm sleep). Bỏ _Thread_local và đoán lại.
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

static int shared = 0;
static _Thread_local int mine = 0;
static pid_t main_pid;

static void *worker(void *arg)
{
    for (int i = 0; i < 3; i++) {
        shared++;
        mine++;
    }
    pid_t tid = (pid_t)syscall(SYS_gettid);
    printf("thread %ld: mine = %d, shared = %d, getpid()==main? %d, tid==pid? %d\n",
           (long)arg, mine, shared, getpid() == main_pid, tid == main_pid);
    return NULL;
}

int main(void)
{
    main_pid = getpid();
    for (long i = 0; i < 2; i++) { /* tạo rồi join ngay → chạy tuần tự, output cố định */
        pthread_t t;
        pthread_create(&t, NULL, worker, (void *)i);
        pthread_join(t, NULL);
    }
    pid_t tid = (pid_t)syscall(SYS_gettid);
    printf("main: mine = %d, shared = %d, tid==pid? %d\n", mine, shared, tid == main_pid);
    return 0;
}
