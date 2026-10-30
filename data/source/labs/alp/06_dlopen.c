/*
 * Lab: Loading a shared library at run time: dlopen, dlsym, dlerror, dlclose (ALP 2.3.6)
 * Topic: ALP chapter 2
 * Predict before running: Can cos be called through the pointer returned by dlsym? What do you get from dlsym on a missing name, or dlopen on a missing library? What does dlerror() return when called twice in a row?
 * Key idea: dlopen(name, RTLD_LAZY | RTLD_NOW) returns a handle (NULL on error); dlsym(handle, "name") returns the symbol's address; read errors with dlerror() (a string; calling it again gives NULL). C++ functions must be declared extern "C" so their names are not mangled. Older glibc needs -ldl at link time.
 * Try next: nm -D /lib/x86_64-linux-gnu/libm.so.6 | grep ' cos$'. ldd ./build/alp/06_dlopen (libm does not show up because it is loaded only at run time).
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

int main(void)
{
    void *handle = dlopen("libm.so.6", RTLD_LAZY);
    if (!handle) { printf("dlopen error: %s\n", dlerror()); return 1; }

    double (*cosine)(double) = (double (*)(double))dlsym(handle, "cos");
    double (*square_root)(double) = (double (*)(double))dlsym(handle, "sqrt");
    printf("cos(0) = %.1f, sqrt(2) = %.4f\n", cosine(0.0), square_root(2.0));

    void *missing = dlsym(handle, "no_such_function");
    printf("dlsym of a missing name: %p\n", missing);
    printf("dlerror() 1st call: %s\n", dlerror());
    const char *again = dlerror();
    printf("dlerror() 2nd call: %s\n", again ? again : "(NULL)");

    void *bad = dlopen("libnosuchlib.so", RTLD_NOW);
    printf("dlopen of a missing library: %p, dlerror: %s\n", bad, dlerror());

    printf("dlclose = %d\n", dlclose(handle));
    return 0;
}
