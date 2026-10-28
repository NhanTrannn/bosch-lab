/*
 * Lab: File descriptors after fork(): a shared offset
 * Topic: Process
 * Predict before running: What does file 1 contain (parent and child share an fd opened BEFORE fork)? What does file 2 contain (each side opens it on its own AFTER fork)?
 * Key idea: fork() duplicates the fd table, but the fds point to the SAME "open file description" in the kernel → shared offset and flags. Separate open() calls create separate descriptions → separate offsets that overwrite each other.
 * Try next: Open file 2 with O_APPEND on both sides and run again. Use dup() instead of fork to see the same shared-offset effect.
 */
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static void dump(const char *path)
{
    char buf[128] = {0};
    int fd = open(path, O_RDONLY);
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    printf("%s (%zd bytes):\n%s", path, n, buf);
}

int main(void)
{
    const char *f1 = "/tmp/bosch_offset1.txt", *f2 = "/tmp/bosch_offset2.txt";

    int fd = open(f1, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fork() == 0) {
        write(fd, "child\n", 6);
        _exit(0);
    }
    wait(NULL);
    write(fd, "parent\n", 7);
    close(fd);
    dump(f1);

    if (fork() == 0) {
        int fd2 = open(f2, O_CREAT | O_TRUNC | O_WRONLY, 0644);
        write(fd2, "XXXXXXXX\n", 9);
        close(fd2);
        _exit(0);
    }
    wait(NULL);
    int fd2 = open(f2, O_WRONLY);
    write(fd2, "parent\n", 7);
    close(fd2);
    dump(f2);

    unlink(f1);
    unlink(f2);
    return 0;
}
