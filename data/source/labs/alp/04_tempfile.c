/*
 * Lab: Safe temporary files: mkstemp + immediate unlink, tmpfile (ALP 2.1.7, listing 2.5)
 * Topic: ALP chapter 2
 * Predict before running: What permissions does mkstemp give the file? After unlink, does access() still see the file, and can you still read through the fd? What is st_nlink before and after unlink?
 * Key idea: mkstemp replaces "XXXXXX" with random characters and creates AND opens the file (O_EXCL) with mode 0600 → its name cannot be guessed or raced. unlink only removes the name; the data lives until the last fd is closed (even if the program crashes). tmpfile() does this for a FILE*. Do not use mktemp/tmpnam/tempnam (there is a race between choosing the name and opening it).
 * Try next: Remove the unlink, then ls -l /tmp/alp_temp.* after the run. While the fd is open, look at ls -l /proc/<pid>/fd (the file shows "(deleted)").
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void)
{
    char temp_filename[] = "/tmp/alp_temp.XXXXXX";
    int fd = mkstemp(temp_filename);
    if (fd == -1) { perror("mkstemp"); return 1; }

    printf("file name: /tmp/alp_temp.<6 chars>, still XXXXXX ? %s, length = %zu\n",
           strstr(temp_filename, "XXXXXX") ? "yes" : "no", strlen(temp_filename));
    struct stat st;
    fstat(fd, &st);
    printf("mode = %o, st_nlink = %lu\n", (unsigned)(st.st_mode & 0777), (unsigned long)st.st_nlink);

    unlink(temp_filename);
    errno = 0;
    int r = access(temp_filename, F_OK);
    printf("after unlink: access() = %d (%s)", r, strerror(errno));
    fstat(fd, &st);
    printf(", st_nlink = %lu\n", (unsigned long)st.st_nlink);

    const char *data = "temporary data";
    size_t length = strlen(data) + 1;
    write(fd, &length, sizeof(length));
    write(fd, data, length);

    lseek(fd, 0, SEEK_SET);
    size_t len2;
    read(fd, &len2, sizeof(len2));
    char *buffer = malloc(len2);
    read(fd, buffer, len2);
    printf("read back through the fd: length = %zu, data = \"%s\"\n", len2, buffer);
    free(buffer);
    close(fd);

    FILE *f = tmpfile();
    fprintf(f, "%d %s", 42, "tmpfile");
    rewind(f);
    int x;
    char word[16];
    fscanf(f, "%d %15s", &x, word);
    printf("tmpfile: read back %d %s, fileno(f) > 2 ? %d\n", x, word, fileno(f) > 2);
    fclose(f);
    return 0;
}
