/*
 * Lab: Mapping a file into memory with mmap
 * Topic: IPC
 * Predict before running: What does the "After" line print? How many bytes is the file?
 * Key idea: mmap with MAP_SHARED on a file: writing to the memory = writing to the file (through the page cache). msync forces it to disk.
 * Try next: Change MAP_SHARED to MAP_PRIVATE; try writing p[st.st_size + 100] (past the file but inside the page) and p[8192].
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(void)
{
    const char *path = "/tmp/bosch_mmap.txt";
    FILE *f = fopen(path, "w");
    fputs("hello mmap world\n", f);
    fclose(f);

    int fd = open(path, O_RDWR);
    struct stat st;
    fstat(fd, &st);
    char *p = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    close(fd);

    printf("Before: %.*s", (int)st.st_size, p);
    for (int i = 0; i < 5; i++)
        p[i] = toupper((unsigned char)p[i]);
    memcpy(p + 6, "MMAP", 4);
    msync(p, st.st_size, MS_SYNC);
    munmap(p, st.st_size);

    char line[64];
    f = fopen(path, "r");
    fgets(line, sizeof(line), f);
    fclose(f);
    printf("After : %s", line);
    printf("File size = %ld bytes\n", (long)st.st_size);
    return 0;
}
