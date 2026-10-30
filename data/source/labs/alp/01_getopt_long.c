/*
 * Lab: getopt_long: short/long options, optarg, optind, "--" and argv permutation (ALP 2.1.3)
 * Topic: ALP chapter 2
 * Predict before running: For each sample argv, which options does getopt_long return, what is optarg, and where does optind stop? How is "-vo x.txt" understood? After "--", is "-h" still an option? What happens with "-x" (not declared) and "-o" without its argument?
 * Key idea: short_options "ho:v": a ':' after a letter means that option needs an argument (optarg). struct option {name, has_arg, flag, val}; the array ends with an all-zero element. getopt_long returns -1 when the options run out; optind is the index of the first non-option argument. GNU getopt permutes argv to move the plain arguments to the end. An invalid option → returns '?' and prints an error to stderr.
 * Try next: Run ./build/alp/01_getopt_long -v --output=a.txt b c to parse a real argv. Set POSIXLY_CORRECT=1 and run sample 4 again (no more permutation).
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

    optind = 0;   /* glibc: 0 = full reinitialization to parse a new argv */
    while ((next_option = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (next_option) {
        case 'h': printf("  -h/--help\n"); break;
        case 'o': printf("  -o/--output, optarg = %s\n", optarg); break;
        case 'v': printf("  -v/--verbose\n"); break;
        case '?': printf("  '?' (optopt = '%c')\n", optopt); break;
        }
        fflush(stdout);
    }
    printf("  optind = %d, remaining arguments:", optind);
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

    parse("Sample 1", 6, a1);
    parse("Sample 2", 4, a2);
    parse("Sample 3", 5, a3);
    parse("Sample 4", 6, a4);
    printf("  argv after permutation:");
    for (int i = 1; i < 6; ++i) printf(" %s", a4[i]);
    printf("\n");
    parse("Sample 5", 3, a5);

    if (argc > 1) parse("real argv", argc, argv);
    return 0;
}
