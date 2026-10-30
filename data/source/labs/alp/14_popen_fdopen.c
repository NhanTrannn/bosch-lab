/*
 * Lab: Pipes with FILE*: fdopen (listing 5.7), dup2 + exec sort (listing 5.8), popen/pclose (listing 5.9)
 * Topic: ALP chapter 5
 * Predict before running: When do the lines sent to sort appear, and in what order? What status does pclose return for popen("...; exit 3", "r")? When does the child's fgets loop on the pipe stop?
 * Key idea: pipe(fds): fds[0] is the read end, fds[1] the write end. fdopen wraps an fd in a FILE* so you can use fprintf/fgets. Close the ends you do not use, otherwise the reader never sees EOF. dup2(fds[0], STDIN_FILENO) then exec → another program reads the pipe as its stdin. popen wraps pipe + fork + exec sh -c; pclose waits for the child and returns its wait status.
 * Try next: Remove close(fds[1]) in the child that runs sort → sort hangs because it never sees EOF. popen("ls -l /", "r") and read it line by line.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int fds[2];
    pid_t pid;

    printf("1) pipe + fdopen:\n");
    fflush(stdout);
    pipe(fds);
    pid = fork();
    if (pid == 0) {
        close(fds[1]);
        FILE *stream = fdopen(fds[0], "r");
        char buffer[64];
        while (fgets(buffer, sizeof buffer, stream) != NULL)
            printf("  [child] got: %s", buffer);
        printf("  [child] fgets returned NULL: EOF because the parent closed the write end\n");
        fclose(stream);
        _exit(0);
    }
    close(fds[0]);
    FILE *stream = fdopen(fds[1], "w");
    for (int i = 1; i <= 3; ++i) fprintf(stream, "Hello, world #%d\n", i);
    fclose(stream);
    waitpid(pid, NULL, 0);

    printf("2) dup2 + execlp(\"sort\"):\n");
    fflush(stdout);
    pipe(fds);
    pid = fork();
    if (pid == 0) {
        close(fds[1]);
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);
        execlp("sort", "sort", (char *)NULL);
        _exit(127);
    }
    close(fds[0]);
    stream = fdopen(fds[1], "w");
    fprintf(stream, "  This is a test.\n  Hello, world.\n  My dog has fleas.\n  This program is great.\n  One fish, two fish.\n");
    fflush(stream);
    printf("  [parent] wrote all 5 lines; sort prints nothing yet because it has not seen EOF\n");
    fflush(stdout);
    close(fds[1]);
    waitpid(pid, NULL, 0);

    printf("3) popen(\"sort\", \"w\"):\n");
    fflush(stdout);
    FILE *out = popen("sort", "w");
    fprintf(out, "  banana\n  apple\n  cherry\n");
    int status = pclose(out);
    printf("  pclose = %d (WEXITSTATUS %d)\n", status, WEXITSTATUS(status));

    printf("4) popen(..., \"r\"):\n");
    FILE *in = popen("echo $((6 * 7)); exit 3", "r");
    char line[32];
    fgets(line, sizeof line, in);
    line[strcspn(line, "\n")] = '\0';
    status = pclose(in);
    printf("  read \"%s\", pclose -> WEXITSTATUS %d\n", line, WEXITSTATUS(status));
    return 0;
}
