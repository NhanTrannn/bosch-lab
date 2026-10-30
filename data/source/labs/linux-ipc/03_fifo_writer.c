/*
 * Lab: Named pipe — the writing side (goes with 03_fifo_reader.c)
 * Topic: IPC
 * Predict before running: see 03_fifo_reader.c
 * Key idea: open(O_WRONLY) blocks until some process opens the FIFO for reading.
 * Try next: run the writer before the reader and see which line blocks.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO "/tmp/bosch_fifo"

int main(void)
{
    if (mkfifo(FIFO, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }
    printf("Writer: opening the FIFO...\n");
    fflush(stdout);
    int fd = open(FIFO, O_WRONLY);
    printf("Writer: opened. Type a few lines, then Ctrl+D to finish\n");

    char line[128];
    while (fgets(line, sizeof(line), stdin))
        write(fd, line, strlen(line));
    close(fd);
    printf("Writer: closed the FIFO\n");
    return 0;
}
