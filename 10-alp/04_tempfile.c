/*
 * Bài: File tạm an toàn: mkstemp + unlink ngay, tmpfile (ALP 2.1.7, listing 2.5)
 * Chủ đề: ALP chương 2
 * Dự đoán trước khi chạy: mkstemp tạo file với quyền gì? Sau unlink, access() thấy file không, còn read qua fd thì sao? st_nlink trước và sau unlink?
 * Ý chính: mkstemp thay "XXXXXX" bằng ký tự ngẫu nhiên, tạo VÀ mở file (O_EXCL) với quyền 0600 → không bị đoán tên hay chen vào giữa. unlink chỉ xoá tên; dữ liệu còn tới khi fd cuối cùng đóng (kể cả khi chương trình crash). tmpfile() làm việc này cho FILE*. Không dùng mktemp/tmpnam/tempnam (có race giữa lúc chọn tên và lúc mở).
 * Thử thêm: Bỏ unlink, chạy xong ls -l /tmp/alp_temp.*. Trong lúc fd còn mở, xem ls -l /proc/<pid>/fd (file hiện "(deleted)").
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

    printf("ten file: /tmp/alp_temp.<6 ky tu>, con XXXXXX ? %s, do dai = %zu\n",
           strstr(temp_filename, "XXXXXX") ? "con" : "khong", strlen(temp_filename));
    struct stat st;
    fstat(fd, &st);
    printf("quyen = %o, st_nlink = %lu\n", (unsigned)(st.st_mode & 0777), (unsigned long)st.st_nlink);

    unlink(temp_filename);
    errno = 0;
    int r = access(temp_filename, F_OK);
    printf("sau unlink: access() = %d (%s)", r, strerror(errno));
    fstat(fd, &st);
    printf(", st_nlink = %lu\n", (unsigned long)st.st_nlink);

    const char *data = "du lieu tam";
    size_t length = strlen(data) + 1;
    write(fd, &length, sizeof(length));
    write(fd, data, length);

    lseek(fd, 0, SEEK_SET);
    size_t len2;
    read(fd, &len2, sizeof(len2));
    char *buffer = malloc(len2);
    read(fd, buffer, len2);
    printf("doc lai qua fd: length = %zu, data = \"%s\"\n", len2, buffer);
    free(buffer);
    close(fd);

    FILE *f = tmpfile();
    fprintf(f, "%d %s", 42, "tmpfile");
    rewind(f);
    int x;
    char word[16];
    fscanf(f, "%d %15s", &x, word);
    printf("tmpfile: doc lai %d %s, fileno(f) > 2 ? %d\n", x, word, fileno(f) > 2);
    fclose(f);
    return 0;
}
