/*
 * Lab: Environment variables (getenv/setenv/environ) and exit codes, including death by signal (ALP 2.1.5, 2.1.6, 3.4)
 * Topic: ALP chapters 2, 3
 * Predict before running: Does setenv(..., 0) overwrite? Do the child process and a program it execs see the variable just set? What WEXITSTATUS does system("exit 300") give? When the shell is killed by SIGTERM, what is the wait status, and what $? does the parent shell see?
 * Key idea: The environment is an array of "NAME=value" strings (environ), COPIED into child processes and kept across exec. setenv only changes the calling process's environment (and of children created afterwards), never the parent shell's. Only the low 8 bits of an exit code survive → use 0..127; when killed by a signal the shell reports 128 + the signal number.
 * Try next: In a terminal: export SERVER_NAME=abc and run the lab again; run a program and echo $? right afterwards.
 */
#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

static void show_status(const char *cmd)
{
    int status = system(cmd);
    printf("system(\"%s\"): ", cmd);
    if (WIFEXITED(status))
        printf("WIFEXITED, WEXITSTATUS = %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("WIFSIGNALED, WTERMSIG = %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
    fflush(stdout);
}

int main(void)
{
    int n = 0;
    for (char **var = environ; *var != NULL; ++var) ++n;
    printf("environ has %s variables, PATH is %s\n", n > 0 ? "> 0" : "0", getenv("PATH") ? "set" : "not set");

    char *server = getenv("SERVER_NAME");
    printf("SERVER_NAME = %s\n", server ? server : "(NULL) -> using the default server.my-company.com");

    setenv("SERVER_NAME", "backup.example.net", 1);
    setenv("SERVER_NAME", "not-overwritten", 0);
    printf("after setenv: SERVER_NAME = %s\n", getenv("SERVER_NAME"));
    fflush(stdout);

    pid_t pid = fork();
    if (pid == 0) {
        printf("child process sees: %s\n", getenv("SERVER_NAME"));
        fflush(stdout);
        execlp("sh", "sh", "-c", "echo \"program after exec sees: $SERVER_NAME\"", (char *)NULL);
        _exit(127);
    }
    waitpid(pid, NULL, 0);

    unsetenv("SERVER_NAME");
    printf("after unsetenv: %s\n", getenv("SERVER_NAME") ? "still set" : "NULL");
    fflush(stdout);

    show_status("exit 3");
    show_status("exit 300");
    show_status("kill -TERM $$");
    show_status("sh -c 'kill -TERM $$'; echo \"  the shell sees \\$? = $?\"; exit 0");
    show_status("no_such_command 2>/dev/null");
    return 0;
}
