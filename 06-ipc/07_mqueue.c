/*
 * Bài: POSIX message queue và độ ưu tiên
 * Chủ đề: IPC
 * Dự đoán trước khi chạy: Con gửi 3 tin với độ ưu tiên 1, 5, 3. Cha nhận theo thứ tự nào? mq_curmsgs trước khi nhận bằng mấy?
 * Ý chính: Message queue giữ RANH GIỚI tin nhắn (khác pipe là luồng byte) và nhận tin có độ ưu tiên cao nhất trước, cùng ưu tiên thì FIFO. Queue tồn tại trong kernel (/dev/mqueue) tới khi mq_unlink, kể cả khi không còn tiến trình nào mở.
 * Thử thêm: Bỏ mq_unlink rồi ls /dev/mqueue sau khi chạy. Gửi quá mq_maxmsg tin → mq_send chặn.
 */
#define _GNU_SOURCE
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NAME "/bosch_mq"

int main(void)
{
    struct mq_attr attr = {.mq_maxmsg = 10, .mq_msgsize = 64};
    mq_unlink(NAME);
    mqd_t q = mq_open(NAME, O_CREAT | O_RDWR, 0600, &attr);
    if (q == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    if (fork() == 0) {
        const char *msg[] = {"thap", "cao", "vua"};
        unsigned prio[] = {1, 5, 3};
        for (int i = 0; i < 3; i++)
            mq_send(q, msg[i], strlen(msg[i]) + 1, prio[i]);
        _exit(0);
    }
    wait(NULL);

    mq_getattr(q, &attr);
    printf("so tin dang cho: %ld\n", attr.mq_curmsgs);
    char buf[64];
    unsigned prio;
    for (int i = 0; i < 3; i++) {
        mq_receive(q, buf, sizeof(buf), &prio);
        printf("nhan \"%s\" (prio %u)\n", buf, prio);
    }
    mq_close(q);
    mq_unlink(NAME);
    return 0;
}
