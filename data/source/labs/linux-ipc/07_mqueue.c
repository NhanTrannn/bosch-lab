/*
 * Lab: POSIX message queues and priorities
 * Topic: IPC
 * Predict before running: The child sends 3 messages with priorities 1, 5 and 3. In what order does the parent receive them? What is mq_curmsgs before receiving?
 * Key idea: A message queue keeps message BOUNDARIES (unlike a pipe, which is a byte stream) and delivers the highest-priority message first, FIFO within a priority. The queue lives in the kernel (/dev/mqueue) until mq_unlink, even when no process has it open.
 * Try next: Remove mq_unlink and ls /dev/mqueue after running it. Send more than mq_maxmsg messages → mq_send blocks.
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
        const char *msg[] = {"low", "high", "medium"};
        unsigned prio[] = {1, 5, 3};
        for (int i = 0; i < 3; i++)
            mq_send(q, msg[i], strlen(msg[i]) + 1, prio[i]);
        _exit(0);
    }
    wait(NULL);

    mq_getattr(q, &attr);
    printf("messages waiting: %ld\n", attr.mq_curmsgs);
    char buf[64];
    unsigned prio;
    for (int i = 0; i < 3; i++) {
        mq_receive(q, buf, sizeof(buf), &prio);
        printf("received \"%s\" (prio %u)\n", buf, prio);
    }
    mq_close(q);
    mq_unlink(NAME);
    return 0;
}
