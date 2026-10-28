/*
 * !!! Needs 2 terminals. Terminal 1: make run T=06-ipc/03_fifo_reader. Terminal 2: make run T=06-ipc/03_fifo_writer.
 * Lab: A named pipe (FIFO) between 2 programs
 * Topic: IPC
 * Predict before running: When only the reader runs, at which line does it stop? When the writer presses Ctrl+D, what does the reader print and how does it end?
 * Key idea: A FIFO is a pipe with a name in the filesystem, letting 2 UNRELATED processes communicate. open() for reading blocks until a writer opens it (and vice versa).
 * Try next: Run ls -l /tmp/bosch_fifo while the reader waits: what is the first character of the permissions? Try echo hello > /tmp/bosch_fifo instead of the writer.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO "/tmp/bosch_fifo"

int main(void)
{
    if (mkfifo(FIFO, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }
    printf("Reader: opening the FIFO...\n");
    fflush(stdout);
    int fd = open(FIFO, O_RDONLY);
    printf("Reader: opened, waiting for data\n");

    char buf[128];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("Reader got: %s", buf);
        fflush(stdout);
    }
    printf("Reader: read returned %zd, exiting\n", n);
    close(fd);
    unlink(FIFO);
    return 0;
}
