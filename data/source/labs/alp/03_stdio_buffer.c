/*
 * Lab: stdio buffering: full, line, unbuffered; stderr is unbuffered; exit vs _exit (ALP 2.1.4)
 * Topic: ALP chapter 2
 * Predict before running: With stdout set to full buffering, in what order do the [stdout] and [stderr] parts appear? In the second part, in what order do A/B/C/D/E appear? Does the last line appear?
 * Key idea: stdout is buffered: to a terminal it is line-buffered (flushed at '
'), to a file/pipe fully buffered (flushed when the buffer fills, on fflush, exit or fclose). stderr is unbuffered and written immediately. exit() flushes the streams; _exit() does not → data still in a buffer is lost.
 * Try next: Run ./build/alp/03_stdio_buffer | cat and ./build/alp/03_stdio_buffer 2>/dev/null. Write the loop while (1) { printf("."); sleep(1); } and then change it to fprintf(stderr, ".").
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    setvbuf(stdout, NULL, _IOFBF, BUFSIZ);   /* full buffering even on a terminal */
    printf("[stdout 1] ");
    fprintf(stderr, "[stderr 2] ");
    printf("[stdout 3]\n");
    fprintf(stderr, "[stderr 4]\n");
    fflush(stdout);

    FILE *full = fdopen(dup(STDOUT_FILENO), "w");
    FILE *line = fdopen(dup(STDOUT_FILENO), "w");
    FILE *none = fdopen(dup(STDOUT_FILENO), "w");
    setvbuf(full, NULL, _IOFBF, BUFSIZ);
    setvbuf(line, NULL, _IOLBF, BUFSIZ);
    setvbuf(none, NULL, _IONBF, 0);

    fprintf(full, "A(full)\n");
    fprintf(line, "B(line) ");
    fprintf(none, "C(none) ");
    fprintf(line, "D(line)\n");
    fprintf(none, "E(none)\n");
    fclose(full);
    fclose(line);
    fclose(none);

    printf("this line sits in the stdout buffer and is lost because of _exit\n");
    _exit(0);
}
