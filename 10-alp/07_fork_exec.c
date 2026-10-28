/*
 * Lab: system, fork + exec (the l/v/p/e exec family), spawn as in listing 3.4, nice (ALP 3.2)
 * Topic: ALP chapter 3
 * Predict before running: Is the line after exec printed? When execvp runs a command that does not exist, what does the child do and what status does the parent see? Is HOME set in the program run by execle? Does nice(10) in the child change the parent's priority?
 * Key idea: fork returns 0 in the child and the child's PID in the parent. exec replaces the whole program of the process (same PID) and only returns on ERROR. Suffixes: l = argument list, v = argv array, p = search PATH, e = pass your own environment. argv[0] is the program name and the list ends with NULL. system() runs /bin/sh -c → convenient but slow and dangerous with user input.
 * Try next: Replace "echo" with "ls" and the arguments "-l", "/". Run nice -n 10 ./build/10-alp/07_fork_exec and look at the parent's nice value.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

/* Listing 3.4: create a child process running program. Returns the child's PID. */
static pid_t spawn(char *program, char **arg_list)
{
    pid_t child_pid = fork();
    if (child_pid != 0)
        return child_pid;
    execvp(program, arg_list);
    fprintf(stderr, "  [child] execvp error: %s\n", strerror(errno));
    abort();
}

static void wait_and_report(pid_t pid)
{
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        printf("  [parent] child %s, exit code %d\n", "exited normally", WEXITSTATUS(status));
    else
        printf("  [parent] child killed by signal %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
    fflush(stdout);
}

int main(void)
{
    printf("1) system:\n");
    fflush(stdout);
    int rc = system("echo \"  [sh] \\$0 = $0\"");
    printf("  system returned %d\n", WEXITSTATUS(rc));

    printf("2) spawn(\"echo\", ...) with fork + execvp:\n");
    fflush(stdout);
    char *echo_args[] = {"echo", "  [echo] hello from the new program", NULL};
    wait_and_report(spawn("echo", echo_args));

    printf("3) spawn a command that does not exist:\n");
    fflush(stdout);
    char *bad_args[] = {"no_such_command", NULL};
    wait_and_report(spawn("no_such_command", bad_args));

    printf("4) execl (full path) and the line after exec:\n");
    fflush(stdout);
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/echo", "echo", "  [echo] run with execl", (char *)NULL);
        printf("  this line is never printed\n");
        _exit(1);
    }
    wait_and_report(pid);

    printf("5) execle with a custom environment:\n");
    fflush(stdout);
    pid = fork();
    if (pid == 0) {
        char *env[] = {"GREETING=hello", NULL};
        execle("/bin/sh", "sh", "-c", "echo \"  [sh] GREETING=$GREETING, HOME=${HOME:-(not set)}\"", (char *)NULL, env);
        _exit(1);
    }
    wait_and_report(pid);

    printf("6) nice:\n");
    fflush(stdout);
    pid = fork();
    if (pid == 0) {
        int before = getpriority(PRIO_PROCESS, 0);
        nice(10);
        printf("  [child] nice: %d -> %d\n", before, getpriority(PRIO_PROCESS, 0));
        _exit(0);
    }
    waitpid(pid, NULL, 0);
    printf("  [parent] nice is still %d\n", getpriority(PRIO_PROCESS, 0));
    return 0;
}
