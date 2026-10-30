/*
 * Lab: mmap: MAP_SHARED and MAP_PRIVATE on a file, msync, anonymous mappings shared through fork (ALP 5.3)
 * Topic: ALP chapter 5
 * Predict before running: Does writing to the MAP_PRIVATE region change the file? What does the MAP_SHARED region show? After writing through MAP_SHARED, what does pread of the file show? The child increments a counter 1000 times in a MAP_SHARED|MAP_ANONYMOUS region: what does the parent see, and with MAP_PRIVATE?
 * Key idea: mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0) maps a file into memory: read and write it like an array; processes mapping the same file see each other's changes → IPC. The file must be big enough first (ftruncate, or lseek + write). MAP_PRIVATE is copy-on-write: writes are only seen by that process and never reach the file. msync(MS_SYNC) pushes changes to disk. munmap removes the mapping.
 * Try next: Remove the ftruncate and touch the mapping (SIGBUS). Change PROT_WRITE to PROT_READ only, then write (SIGSEGV).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define FILE_LENGTH 4096

static void file_content(int fd, char *out)
{
    pread(fd, out, 32, 0);
    out[31] = '\0';
}

int main(void)
{
    char name[] = "/tmp/alp_mmap.XXXXXX", buf[32];
    int fd = mkstemp(name);
    unlink(name);
    ftruncate(fd, FILE_LENGTH);
    pwrite(fd, "original", 9, 0);

    char *shared = mmap(NULL, FILE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    char *priv = mmap(NULL, FILE_LENGTH, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    printf("at first: shared = \"%s\", private = \"%s\"\n", shared, priv);

    strcpy(priv, "via PRIVATE");
    file_content(fd, buf);
    printf("write via MAP_PRIVATE: private = \"%s\", shared = \"%s\", file = \"%s\"\n", priv, shared, buf);

    strcpy(shared, "via SHARED");
    printf("msync = %d\n", msync(shared, FILE_LENGTH, MS_SYNC));
    file_content(fd, buf);
    printf("write via MAP_SHARED: shared = \"%s\", file = \"%s\", private = \"%s\"\n", shared, buf, priv);

    munmap(shared, FILE_LENGTH);
    munmap(priv, FILE_LENGTH);
    close(fd);

    int *sh = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *pv = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    *sh = *pv = 0;
    if (fork() == 0) {
        for (int i = 0; i < 1000; ++i) { ++*sh; ++*pv; }
        _exit(0);
    }
    wait(NULL);
    printf("after the child incremented 1000 times: MAP_SHARED = %d, MAP_PRIVATE = %d\n", *sh, *pv);
    return 0;
}
