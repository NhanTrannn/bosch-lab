/*
 * Lab: errno, strerror, perror and assert (including NDEBUG) (ALP 2.2)
 * Topic: ALP chapter 2
 * Predict before running: What is errno after each failing call? After a SUCCESSFUL call, does errno go back to 0? With NDEBUG, how many times does assert(do_something() == 0) call the function? How does the last line end the program?
 * Key idea: A failing system call returns -1 (or NULL) and sets errno; success does NOT clear errno → only read errno right after a call reports failure, and save it before calling anything else. strerror(errno) turns the code into a string, perror prints to stderr. A false assert(expr) prints file:line and calls abort(); -DNDEBUG removes asserts entirely, so the expression inside assert must have no side effects.
 * !!! The lab deliberately ends with SIGABRT at the last assert: the shell reports "Aborted (core dumped)", exit code 134.
 * Try next: Read /usr/include/asm-generic/errno-base.h. Rebuild with -DNDEBUG and check whether the last assert still aborts.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int calls = 0;
static int do_something(void) { ++calls; return 0; }

static void report(const char *what, int ret)
{
    int saved = errno;   /* save it right away; printf may change errno */
    printf("%-28s -> %2d, errno = %2d %-7s (%s)\n", what, ret, saved,
           saved == ENOENT ? "ENOENT" : saved == EISDIR ? "EISDIR" : saved == EBADF ? "EBADF" :
           saved == EACCES ? "EACCES" : saved == EEXIST ? "EEXIST" : saved == EINVAL ? "EINVAL" : "",
           strerror(saved));
}

#define NDEBUG
#include <assert.h>
static void with_ndebug(void)
{
    for (int i = 0; i < 3; ++i) assert(do_something() == 0);
}

#undef NDEBUG
#include <assert.h>   /* assert.h is designed to be re-included; it re-reads NDEBUG each time */
static void without_ndebug(void)
{
    for (int i = 0; i < 3; ++i) assert(do_something() == 0);
}

int main(void)
{
    report("open(\"/does/not/exist\")", open("/does/not/exist", O_RDONLY));
    report("getpid() (succeeds)", getpid() > 0);
    report("open(\"/tmp\", O_WRONLY)", open("/tmp", O_WRONLY));
    report("close(12345)", close(12345));
    report("mkdir(\"/tmp\")", mkdir("/tmp", 0755));
    report("lseek(0, 0, 12345)", (int)lseek(0, 0, 12345));
    if (geteuid() != 0) report("open(\"/etc/shadow\")", open("/etc/shadow", O_RDONLY));
    fflush(stdout);

    errno = ENOENT;
    perror("perror prints to stderr");

    with_ndebug();
    printf("NDEBUG: calls = %d\n", calls);
    without_ndebug();
    printf("without NDEBUG: calls = %d\n", calls);
    fflush(stdout);

    const char *pointer = NULL;
    assert(pointer != NULL);
    printf("never printed\n");
    return 0;
}
