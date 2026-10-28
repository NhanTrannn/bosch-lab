/*
 * Bài: Nạp thư viện động lúc chạy: dlopen, dlsym, dlerror, dlclose (ALP 2.3.6)
 * Chủ đề: ALP chương 2
 * Dự đoán trước khi chạy: Gọi cos qua con trỏ lấy từ dlsym được không? dlsym một tên không có, hoặc dlopen một thư viện không có thì nhận gì? dlerror() gọi hai lần liên tiếp trả về gì?
 * Ý chính: dlopen(tên, RTLD_LAZY | RTLD_NOW) trả về handle (NULL nếu lỗi); dlsym(handle, "tên") trả về địa chỉ symbol; lỗi đọc bằng dlerror() (chuỗi, gọi lại lần nữa thì NULL). Hàm C++ phải khai báo extern "C" để tên không bị mangle. Glibc cũ cần -ldl khi link.
 * Thử thêm: nm -D /lib/x86_64-linux-gnu/libm.so.6 | grep ' cos$'. ldd ./build/10-alp/06_dlopen (không thấy libm vì chỉ nạp lúc chạy).
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

int main(void)
{
    void *handle = dlopen("libm.so.6", RTLD_LAZY);
    if (!handle) { printf("dlopen loi: %s\n", dlerror()); return 1; }

    double (*cosine)(double) = (double (*)(double))dlsym(handle, "cos");
    double (*square_root)(double) = (double (*)(double))dlsym(handle, "sqrt");
    printf("cos(0) = %.1f, sqrt(2) = %.4f\n", cosine(0.0), square_root(2.0));

    void *missing = dlsym(handle, "khong_co_ham_nay");
    printf("dlsym ten khong co: %p\n", missing);
    printf("dlerror() lan 1: %s\n", dlerror());
    const char *again = dlerror();
    printf("dlerror() lan 2: %s\n", again ? again : "(NULL)");

    void *bad = dlopen("libkhongco.so", RTLD_NOW);
    printf("dlopen thu vien khong co: %p, dlerror: %s\n", bad, dlerror());

    printf("dlclose = %d\n", dlclose(handle));
    return 0;
}
