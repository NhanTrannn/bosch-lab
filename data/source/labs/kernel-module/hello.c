/*
 * !!! Do NOT build with make at the top level. cd into data/source/labs/kernel-module and run make there. insmod/rmmod need sudo.
 * Lab: Hello kernel module
 * Topic: Kernel
 * Predict before running: After insmod, what does dmesg | tail print? What about sudo insmod hello.ko count=3? What does rmmod print?
 * Key idea: A module has an init function (runs on insmod) and an exit function (runs on rmmod). Log with printk/pr_info and read the log with dmesg. The kernel has no libc (no printf, no malloc).
 * Try next: modinfo hello.ko; cat /sys/module/hello/parameters/count while the module is loaded.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bosch-lab");
MODULE_DESCRIPTION("Hello world kernel module");

static int count = 1;
module_param(count, int, 0444);
MODULE_PARM_DESC(count, "Number of times to print the greeting");

static int __init hello_init(void)
{
    int i;

    for (i = 0; i < count; i++)
        pr_info("hello: hello kernel (%d)\n", i);
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("hello: goodbye kernel\n");
}

module_init(hello_init);
module_exit(hello_exit);
