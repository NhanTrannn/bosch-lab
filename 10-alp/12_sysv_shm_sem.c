/*
 * Bài: System V shared memory và semaphore: shmget/shmat/shmctl, semget/semop, SEM_UNDO (ALP 5.1, 5.2)
 * Chủ đề: ALP chương 5
 * Dự đoán trước khi chạy: shm_nattch bằng mấy trước fork, trong lúc con còn sống, sau khi con thoát? Cha đọc được chuỗi con ghi vào vùng nhớ chung không? semop -1 với IPC_NOWAIT khi giá trị là 0? Con tăng semaphore với SEM_UNDO rồi thoát thì giá trị còn lại bao nhiêu?
 * Ý chính: shmget tạo (hoặc lấy) segment theo key (IPC_PRIVATE = luôn tạo mới), shmat gắn vào không gian địa chỉ, shmdt gỡ. Segment KHÔNG tự mất khi process thoát; phải shmctl(IPC_RMID) (hoặc ipcrm), xem bằng ipcs -m. Shared memory không tự đồng bộ → cần semaphore. semop với sem_op âm = wait, dương = post; SEM_UNDO để kernel hoàn tác khi process chết.
 * Thử thêm: Bỏ hai dòng IPC_RMID, chạy xong gõ ipcs -m và ipcs -s, rồi xoá bằng ipcrm -m <shmid> / ipcrm -s <semid>.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

union semun { int val; struct semid_ds *buf; unsigned short *array; };

static unsigned long nattch(int shmid)
{
    struct shmid_ds ds;
    shmctl(shmid, IPC_STAT, &ds);
    return (unsigned long)ds.shm_nattch;
}

static int sem_change(int semid, int delta, int flags)
{
    struct sembuf op = {0, (short)delta, (short)flags};
    return semop(semid, &op, 1);
}

int main(void)
{
    int segment_id = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    char *shared_memory = shmat(segment_id, NULL, 0);
    struct shmid_ds ds;
    shmctl(segment_id, IPC_STAT, &ds);
    printf("segment: size = %zu (= getpagesize()), nattch = %lu\n", (size_t)ds.shm_segsz, nattch(segment_id));

    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    union semun arg = {.val = 0};
    semctl(semid, 0, SETVAL, arg);

    errno = 0;
    int rc = sem_change(semid, -1, IPC_NOWAIT);
    printf("semop -1 IPC_NOWAIT khi = 0: %d, errno = %s\n", rc, errno == EAGAIN ? "EAGAIN" : strerror(errno));
    fflush(stdout);

    pid_t pid = fork();
    if (pid == 0) {
        sprintf(shared_memory, "Hello, world (tu con)");
        printf("  [con] da ghi, nattch = %lu\n", nattch(segment_id));
        fflush(stdout);
        sem_change(semid, +1, 0);          /* post: báo cha đọc được */
        sem_change(semid, -1, 0);          /* chờ cha đọc xong */
        _exit(0);                          /* thoát → tự shmdt */
    }
    sem_change(semid, -1, 0);              /* wait */
    printf("  [cha] doc duoc: \"%s\"\n", shared_memory);
    fflush(stdout);
    sem_change(semid, +1, 0);
    waitpid(pid, NULL, 0);
    printf("sau khi con thoat: nattch = %lu\n", nattch(segment_id));

    pid = fork();
    if (pid == 0) {
        sem_change(semid, +3, SEM_UNDO);
        printf("  [con] +3 voi SEM_UNDO, gia tri = %d\n", semctl(semid, 0, GETVAL));
        fflush(stdout);
        _exit(0);
    }
    waitpid(pid, NULL, 0);
    printf("con thoat, kernel hoan tac: gia tri = %d\n", semctl(semid, 0, GETVAL));

    shmdt(shared_memory);
    printf("sau shmdt cua cha: nattch = %lu\n", nattch(segment_id));
    shmctl(segment_id, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    errno = 0;
    shmctl(segment_id, IPC_STAT, &ds);
    printf("sau IPC_RMID: shmctl IPC_STAT -> %s\n", errno == EINVAL || errno == EIDRM ? "EINVAL/EIDRM (da xoa)" : strerror(errno));
    return 0;
}
