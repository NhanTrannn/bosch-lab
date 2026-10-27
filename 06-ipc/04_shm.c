/*
 * Bài: POSIX shared memory giữa cha và con
 * Chủ đề: IPC
 * Dự đoán trước khi chạy: Cha đọc được chuỗi nào trong mem? Biến local ở cha bằng mấy?
 * Ý chính: Vùng mmap MAP_SHARED được chia sẻ thật giữa các tiến trình; còn biến thường sau fork là bản copy (copy-on-write).
 * Thử thêm: Bỏ dòng shm_unlink rồi chạy, sau đó ls -l /dev/shm. Đổi MAP_SHARED thành MAP_PRIVATE và đoán lại.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define NAME "/bosch_shm"
#define SIZE 4096

int main(void)
{
    int fd = shm_open(NAME, O_CREAT | O_RDWR, 0600);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }
    if (ftruncate(fd, SIZE) == -1)
        perror("ftruncate");
    char *mem = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    strcpy(mem, "ban dau");

    int local = 1;
    pid_t pid = fork();
    if (pid == 0) {
        strcpy(mem, "con da ghi vao shm");
        local = 99;
        printf("Con: mem = \"%s\", local = %d\n", mem, local);
        return 0;
    }
    waitpid(pid, NULL, 0);
    printf("Cha: mem = \"%s\", local = %d\n", mem, local);

    munmap(mem, SIZE);
    shm_unlink(NAME);
    return 0;
}
