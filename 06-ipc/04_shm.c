/*
 * Lab: POSIX shared memory between a parent and a child
 * Topic: IPC
 * Predict before running: Which string does the parent read in mem? What is the local variable in the parent?
 * Key idea: A MAP_SHARED mmap region is really shared between processes; a normal variable after fork is a copy (copy-on-write).
 * Try next: Remove the shm_unlink line, run it, then ls -l /dev/shm. Change MAP_SHARED to MAP_PRIVATE and predict again.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define NAME "/bosch_shm"
#define SIZE 4096

int main(void)
{
    int fd = shm_open(NAME, O_CREAT | O_RDWR, 0600);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }
    if (ftruncate(fd, SIZE) == -1)
        perror("ftruncate");
    char *mem = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    strcpy(mem, "initial");

    int local = 1;
    pid_t pid = fork();
    if (pid == 0) {
        strcpy(mem, "child wrote to shm");
        local = 99;
        printf("Child:  mem = \"%s\", local = %d\n", mem, local);
        return 0;
    }
    waitpid(pid, NULL, 0);
    printf("Parent: mem = \"%s\", local = %d\n", mem, local);

    munmap(mem, SIZE);
    shm_unlink(NAME);
    return 0;
}
