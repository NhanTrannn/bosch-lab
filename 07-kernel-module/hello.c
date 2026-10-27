/*
 * !!! KHÔNG build bằng make ở gốc. Vào thư mục 07-kernel-module rồi make. Cần sudo để insmod/rmmod.
 * Bài: Kernel module Hello
 * Chủ đề: Kernel
 * Dự đoán trước khi chạy: Sau insmod, dmesg | tail in gì? Với sudo insmod hello.ko count=3 thì sao? rmmod in gì?
 * Ý chính: Module có hàm init (chạy khi insmod) và exit (khi rmmod). In log bằng printk/pr_info, xem bằng dmesg. Kernel không có libc (không printf, malloc).
 * Thử thêm: modinfo hello.ko; cat /sys/module/hello/parameters/count khi module đang nạp.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bosch-lab");
MODULE_DESCRIPTION("Hello world kernel module");

static int count = 1;
module_param(count, int, 0444);
MODULE_PARM_DESC(count, "So lan in loi chao");

static int __init hello_init(void)
{
    int i;

    for (i = 0; i < count; i++)
        pr_info("hello: xin chao kernel (%d)\n", i);
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("hello: tam biet kernel\n");
}

module_init(hello_init);
module_exit(hello_exit);
