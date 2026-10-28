/*
 * Bài: Buffer của stdio: full, line, không buffer; stderr không buffer; exit và _exit (ALP 2.1.4)
 * Chủ đề: ALP chương 2
 * Dự đoán trước khi chạy: Với stdout đặt full buffering, các dòng [stdout] và [stderr] hiện theo thứ tự nào? Ở phần hai, chữ A/B/C/D/E hiện theo thứ tự nào? Dòng cuối có hiện ra không?
 * Ý chính: stdout có buffer: ra terminal thì line-buffered (đẩy khi gặp '\n'), ra file/pipe thì full-buffered (đẩy khi đầy buffer, fflush, exit hoặc fclose). stderr không buffer, ghi ra ngay. exit() flush các stream; _exit() thì không → dữ liệu trong buffer bị mất.
 * Thử thêm: Chạy ./build/10-alp/03_stdio_buffer | cat và ./build/10-alp/03_stdio_buffer 2>/dev/null. Viết vòng while (1) { printf("."); sleep(1); } rồi đổi thành fprintf(stderr, ".").
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    setvbuf(stdout, NULL, _IOFBF, BUFSIZ);   /* full buffering kể cả khi ra terminal */
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

    printf("dong nay nam trong buffer cua stdout va se bi mat vi _exit\n");
    _exit(0);
}
