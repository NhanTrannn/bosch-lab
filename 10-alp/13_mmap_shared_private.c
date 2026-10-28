/*
 * Bài: mmap: MAP_SHARED và MAP_PRIVATE trên file, msync, anonymous mapping chia sẻ qua fork (ALP 5.3)
 * Chủ đề: ALP chương 5
 * Dự đoán trước khi chạy: Ghi vào vùng MAP_PRIVATE có làm đổi file không? Vùng MAP_SHARED thấy gì? Sau khi ghi qua MAP_SHARED, đọc file bằng pread thấy gì? Con tăng counter trong vùng MAP_SHARED|MAP_ANONYMOUS, cha thấy bao nhiêu, còn với MAP_PRIVATE?
 * Ý chính: mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0) ánh xạ file vào bộ nhớ: đọc/ghi như mảng, nhiều process map cùng file thấy thay đổi của nhau → dùng làm IPC. File phải đủ lớn trước (ftruncate hoặc lseek + write). MAP_PRIVATE là copy-on-write: ghi chỉ thấy trong process đó, không ghi xuống file. msync(MS_SYNC) đẩy thay đổi xuống đĩa. munmap gỡ ánh xạ.
 * Thử thêm: Bỏ ftruncate rồi truy cập vùng map (SIGBUS). Đổi PROT_WRITE thành chỉ PROT_READ rồi ghi (SIGSEGV).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define FILE_LENGTH 4096

static void file_content(int fd, char *out)
{
    pread(fd, out, 32, 0);
    out[31] = '\0';
}

int main(void)
{
    char name[] = "/tmp/alp_mmap.XXXXXX", buf[32];
    int fd = mkstemp(name);
    unlink(name);
    ftruncate(fd, FILE_LENGTH);
    pwrite(fd, "ban goc", 8, 0);

    char *shared = mmap(NULL, FILE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    char *priv = mmap(NULL, FILE_LENGTH, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    printf("ban dau: shared = \"%s\", private = \"%s\"\n", shared, priv);

    strcpy(priv, "sua qua PRIVATE");
    file_content(fd, buf);
    printf("ghi qua MAP_PRIVATE: private = \"%s\", shared = \"%s\", file = \"%s\"\n", priv, shared, buf);

    strcpy(shared, "sua qua SHARED");
    printf("msync = %d\n", msync(shared, FILE_LENGTH, MS_SYNC));
    file_content(fd, buf);
    printf("ghi qua MAP_SHARED: shared = \"%s\", file = \"%s\", private = \"%s\"\n", shared, buf, priv);

    munmap(shared, FILE_LENGTH);
    munmap(priv, FILE_LENGTH);
    close(fd);

    int *sh = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *pv = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    *sh = *pv = 0;
    if (fork() == 0) {
        for (int i = 0; i < 1000; ++i) { ++*sh; ++*pv; }
        _exit(0);
    }
    wait(NULL);
    printf("sau khi con tang 1000 lan: MAP_SHARED = %d, MAP_PRIVATE = %d\n", *sh, *pv);
    return 0;
}
