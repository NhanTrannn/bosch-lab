# Kernel module — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

### hello — Hello kernel module

**Question:** After insmod, what does dmesg | tail print? What about sudo insmod hello.ko count=3? What does rmmod print?

**Expected output:**

```text
$ sudo apt install build-essential linux-headers-$(uname -r)
$ make
$ sudo insmod hello.ko
$ dmesg | tail -2
[12345.678901] hello: hello kernel (0)
$ lsmod | grep hello
hello                  16384  0
$ sudo rmmod hello
$ dmesg | tail -1
[12350.123456] hello: goodbye kernel

$ sudo insmod hello.ko count=3   -> prints 3 lines (0), (1), (2)

With Secure Boot on: insmod: ERROR: could not insert module hello.ko: Key was rejected by service
(being able to build hello.ko is enough for exam preparation)
```

**Explanation:**

- `module_init/module_exit` declare the entry and exit points. `__init` lets the kernel free the init code after loading.
- Without `MODULE_LICENSE("GPL")` the kernel becomes "tainted" and the module cannot use `EXPORT_SYMBOL_GPL` symbols.
- `insmod` loads exactly the given file; `modprobe` searches `/lib/modules` and loads dependencies automatically.
- If init returns a negative value (e.g. `-ENOMEM`), insmod fails and the module is not loaded.

**Exam relevance:** Common questions: the command that lists loaded modules (`lsmod` or `cat /proc/modules`), how to read the kernel log (`dmesg`), the module file extension (`.ko`), the kernel's logging function (`printk`).
