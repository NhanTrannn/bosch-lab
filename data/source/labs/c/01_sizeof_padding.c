/*
 * Lab: sizeof and struct padding
 * Topic: C
 * Predict before running: What are sizeof(struct A), sizeof(struct B) and sizeof(struct C)? What is the offset of each member?
 * Key idea: The compiler inserts padding so each member sits at its own alignment; the whole struct is also rounded up to a multiple of its largest alignment.
 * Try next: Change int to double (alignment 8) or add a short member, then predict again.
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
