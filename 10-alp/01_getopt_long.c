/*
 * Bài: getopt_long: tùy chọn ngắn/dài, optarg, optind, "--" và hoán vị argv (ALP 2.1.3)
 * Chủ đề: ALP chương 2
 * Dự đoán trước khi chạy: Với mỗi argv mẫu, getopt_long trả về những tùy chọn nào, optarg là gì, optind dừng ở đâu? "-vo x.txt" được hiểu thế nào? Sau "--" thì "-h" còn là tùy chọn không? Gặp "-x" (không khai báo) và "-o" thiếu đối số thì sao?
 * Ý chính: short_options "ho:v": dấu ':' sau chữ cái nghĩa là tùy chọn đó cần đối số (optarg). struct option {name, has_arg, flag, val}, mảng kết thúc bằng phần tử toàn 0. getopt_long trả về -1 khi hết tùy chọn; optind là chỉ số của đối số không phải tùy chọn đầu tiên. GNU getopt hoán vị argv để đưa các đối số thường xuống cuối. Tùy chọn sai → trả về '?' và in lỗi ra stderr.
 * Thử thêm: Chạy ./build/10-alp/01_getopt_long -v --output=a.txt b c để xem chương trình phân tích argv thật. Đặt POSIXLY_CORRECT=1 rồi chạy lại mẫu 4 (không hoán vị nữa).
 */
#define _GNU_SOURCE
#include <getopt.h>
#include <stdio.h>

static void parse(const char *title, int argc, char *argv[])
{
    const char *const short_options = "ho:v";
    const struct option long_options[] = {
        {"help", 0, NULL, 'h'},
        {"output", 1, NULL, 'o'},
        {"verbose", 0, NULL, 'v'},
        {NULL, 0, NULL, 0},
    };
    int next_option;

    printf("%s:", title);
    for (int i = 1; i < argc; ++i) printf(" %s", argv[i]);
    printf("\n");
    fflush(stdout);

    optind = 0;   /* glibc: 0 = khởi tạo lại hoàn toàn để phân tích argv mới */
    while ((next_option = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (next_option) {
        case 'h': printf("  -h/--help\n"); break;
        case 'o': printf("  -o/--output, optarg = %s\n", optarg); break;
        case 'v': printf("  -v/--verbose\n"); break;
        case '?': printf("  '?' (optopt = '%c')\n", optopt); break;
        }
        fflush(stdout);
    }
    printf("  optind = %d, doi so con lai:", optind);
    for (int i = optind; i < argc; ++i) printf(" %s", argv[i]);
    printf("\n");
}

int main(int argc, char *argv[])
{
    char *a1[] = {"prog", "-v", "-o", "out.txt", "in1", "in2", NULL};
    char *a2[] = {"prog", "--output=log.txt", "--verbose", "file", NULL};
    char *a3[] = {"prog", "-vo", "x.txt", "--", "-h", NULL};
    char *a4[] = {"prog", "in1", "-v", "in2", "--output", "o.txt", NULL};
    char *a5[] = {"prog", "-x", "-o", NULL};

    parse("Mau 1", 6, a1);
    parse("Mau 2", 4, a2);
    parse("Mau 3", 5, a3);
    parse("Mau 4", 6, a4);
    printf("  argv sau khi hoan vi:");
    for (int i = 1; i < 6; ++i) printf(" %s", a4[i]);
    printf("\n");
    parse("Mau 5", 3, a5);

    if (argc > 1) parse("argv that", argc, argv);
    return 0;
}
