/*
 * Bài: Ánh xạ file vào bộ nhớ bằng mmap
 * Chủ đề: IPC
 * Dự đoán trước khi chạy: Dòng "Sau" in gì? Kích thước file là bao nhiêu byte?
 * Ý chính: mmap MAP_SHARED trên file: ghi vào bộ nhớ = ghi vào file (qua page cache). msync ép ghi xuống đĩa.
 * Thử thêm: Đổi MAP_SHARED thành MAP_PRIVATE; thử ghi p[st.st_size + 100] (ngoài file nhưng trong trang) và p[8192].
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(void)
{
    const char *path = "/tmp/bosch_mmap.txt";
    FILE *f = fopen(path, "w");
    fputs("hello mmap world\n", f);
    fclose(f);

    int fd = open(path, O_RDWR);
    struct stat st;
    fstat(fd, &st);
    char *p = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    close(fd);

    printf("Truoc: %.*s", (int)st.st_size, p);
    for (int i = 0; i < 5; i++)
        p[i] = toupper((unsigned char)p[i]);
    memcpy(p + 6, "MMAP", 4);
    msync(p, st.st_size, MS_SYNC);
    munmap(p, st.st_size);

    char line[64];
    f = fopen(path, "r");
    fgets(line, sizeof(line), f);
    fclose(f);
    printf("Sau  : %s", line);
    printf("File size = %ld byte\n", (long)st.st_size);
    return 0;
}
