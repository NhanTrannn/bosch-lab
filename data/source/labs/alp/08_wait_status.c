/*
 * Lab: wait/waitpid and the status macros, WNOHANG, zombies in /proc (ALP 3.4)
 * Topic: ALP chapter 3
 * Predict before running: What WEXITSTATUS does exit(300) give? What do WIFSIGNALED/WTERMSIG report for abort() and kill -9? What does waitpid(..., WNOHANG) return while the child is still running? What state does /proc show for a child that exited but was not waited for? What about wait() when there are no children?
 * Key idea: Only the low 8 bits of the exit code survive. WIFEXITED/WEXITSTATUS for a normal exit, WIFSIGNALED/WTERMSIG for death by signal. WNOHANG does not block and returns 0 if no child has finished. A child that ended without being waited for → a zombie (state Z) holding only an entry in the process table; after wait it disappears.
 * Try next: While the lab sleeps, run ps -o pid,ppid,stat,cmd --ppid <parent pid> in Terminal 2. Replace abort() with *(int *)0 = 1.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void describe(const char *label, int status)
{
    printf("%-22s ", label);
    if (WIFEXITED(status))
        printf("WIFEXITED   WEXITSTATUS = %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("WIFSIGNALED WTERMSIG = %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
    fflush(stdout);
}

static char proc_state(pid_t pid)
{
    char path[64], buf[256], state = '?';
    snprintf(path, sizeof path, "/proc/%d/stat", pid);
    FILE *f = fopen(path, "r");
    if (!f) return '-';
    if (fgets(buf, sizeof buf, f)) {
        char *p = strrchr(buf, ')');
        if (p && p[1] == ' ') state = p[2];
    }
    fclose(f);
    return state;
}

int main(void)
{
    int status;
    pid_t pid;

    if ((pid = fork()) == 0) exit(300);
    waitpid(pid, &status, 0);
    describe("child exit(300):", status);

    if ((pid = fork()) == 0) exit(-1);
    waitpid(pid, &status, 0);
    describe("child exit(-1):", status);

    if ((pid = fork()) == 0) abort();
    waitpid(pid, &status, 0);
    describe("child abort():", status);

    if ((pid = fork()) == 0) { pause(); _exit(0); }
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
    describe("child kill -9:", status);

    if ((pid = fork()) == 0) { usleep(300000); _exit(7); }
    printf("WNOHANG while the child runs: waitpid = %d\n", waitpid(pid, &status, WNOHANG));
    fflush(stdout);
    usleep(500000);
    printf("child exited, not waited for: state in /proc = %c\n", proc_state(pid));
    fflush(stdout);
    pid_t got = waitpid(pid, &status, WNOHANG);
    printf("WNOHANG 2nd call: returned the child's PID ? %d\n", got == pid);
    describe("child exit(7):", status);
    printf("after wait: /proc/<pid> = %c (gone)\n", proc_state(pid));

    errno = 0;
    int rc = wait(&status);   /* a separate call: the evaluation order of printf's arguments is unspecified */
    printf("wait() with no children left: %d, errno = %s\n", rc, errno == ECHILD ? "ECHILD" : strerror(errno));
    return 0;
}
