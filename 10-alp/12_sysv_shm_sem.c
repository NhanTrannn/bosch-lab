/*
 * Lab: System V shared memory and semaphores: shmget/shmat/shmctl, semget/semop, SEM_UNDO (ALP 5.1, 5.2)
 * Topic: ALP chapter 5
 * Predict before running: What is shm_nattch before fork, while the child is alive, and after the child exits? Can the parent read the string the child wrote into shared memory? semop -1 with IPC_NOWAIT when the value is 0? The child adds to the semaphore with SEM_UNDO and exits: what value is left?
 * Key idea: shmget creates (or gets) a segment by key (IPC_PRIVATE = always a new one), shmat attaches it to the address space, shmdt detaches. A segment does NOT disappear when processes exit; you must shmctl(IPC_RMID) it (or ipcrm), and ipcs -m lists them. Shared memory is not synchronized → use a semaphore. semop with a negative sem_op = wait, positive = post; SEM_UNDO makes the kernel undo the change when the process dies.
 * Try next: Remove the two IPC_RMID lines, then run ipcs -m and ipcs -s afterwards, and remove them with ipcrm -m <shmid> / ipcrm -s <semid>.
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
    printf("semop -1 IPC_NOWAIT at 0: %d, errno = %s\n", rc, errno == EAGAIN ? "EAGAIN" : strerror(errno));
    fflush(stdout);

    pid_t pid = fork();
    if (pid == 0) {
        sprintf(shared_memory, "Hello, world (from the child)");
        printf("  [child] wrote it, nattch = %lu\n", nattch(segment_id));
        fflush(stdout);
        sem_change(semid, +1, 0);          /* post: tell the parent it can read */
        sem_change(semid, -1, 0);          /* wait until the parent has read it */
        _exit(0);                          /* exiting → automatic shmdt */
    }
    sem_change(semid, -1, 0);              /* wait */
    printf("  [parent] read: \"%s\"\n", shared_memory);
    fflush(stdout);
    sem_change(semid, +1, 0);
    waitpid(pid, NULL, 0);
    printf("after the child exited: nattch = %lu\n", nattch(segment_id));

    pid = fork();
    if (pid == 0) {
        sem_change(semid, +3, SEM_UNDO);
        printf("  [child] +3 with SEM_UNDO, value = %d\n", semctl(semid, 0, GETVAL));
        fflush(stdout);
        _exit(0);
    }
    waitpid(pid, NULL, 0);
    printf("child exited, the kernel undid it: value = %d\n", semctl(semid, 0, GETVAL));

    shmdt(shared_memory);
    printf("after the parent's shmdt: nattch = %lu\n", nattch(segment_id));
    shmctl(segment_id, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    errno = 0;
    shmctl(segment_id, IPC_STAT, &ds);
    printf("after IPC_RMID: shmctl IPC_STAT -> %s\n", errno == EINVAL || errno == EIDRM ? "EINVAL/EIDRM (removed)" : strerror(errno));
    return 0;
}
