/*
 * Bài: sizeof và padding của struct
 * Chủ đề: C
 * Dự đoán trước khi chạy: sizeof(struct A), sizeof(struct B), sizeof(struct C) là bao nhiêu? Offset từng member?
 * Ý chính: Compiler chèn padding để mỗi member nằm đúng alignment của nó; cả struct cũng được làm tròn lên bội số alignment lớn nhất.
 * Thử thêm: Đổi int thành double (alignment 8) hoặc thêm một member short rồi đoán lại.
 */
#include <stdio.h>
#include <stddef.h>

struct A { char c1; int i; char c2; };
struct B { int i; char c1; char c2; };

#pragma pack(push, 1)
struct C { char c1; int i; char c2; };
#pragma pack(pop)

int main(void)
{
    printf("sizeof(A) = %zu | c1=%zu i=%zu c2=%zu\n", sizeof(struct A),
           offsetof(struct A, c1), offsetof(struct A, i), offsetof(struct A, c2));
    printf("sizeof(B) = %zu | i=%zu c1=%zu c2=%zu\n", sizeof(struct B),
           offsetof(struct B, i), offsetof(struct B, c1), offsetof(struct B, c2));
    printf("sizeof(C) = %zu | c1=%zu i=%zu c2=%zu\n", sizeof(struct C),
           offsetof(struct C, c1), offsetof(struct C, i), offsetof(struct C, c2));
    return 0;
}
