# Linux kernel and embedded systems

## Theory

### Architecture
- **Monolithic kernel** (Linux): scheduler, memory management, filesystems and drivers all run in kernel space → fast, but a driver bug can bring the whole system down. Linux supports **loadable modules** that are inserted/removed at run time.
- **Microkernel** (QNX, seL4): keeps only the minimum in the kernel; drivers/services run in user space → more stable and secure, slower because of IPC.
- User space ↔ kernel space via system calls; the kernel cannot use libc (`printk` instead of `printf`, `kmalloc` instead of `malloc`).

### Kernel modules
```c
#include <linux/module.h>
static int __init hello_init(void) { pr_info("hello\n"); return 0; }
static void __exit hello_exit(void) { pr_info("bye\n"); }
module_init(hello_init); module_exit(hello_exit);
MODULE_LICENSE("GPL");
```
- Built with Kbuild (`obj-m += hello.o`, `make -C /lib/modules/$(uname -r)/build M=$PWD modules`) → `hello.ko`.
- `insmod` loads (no dependency handling), `modprobe` loads with dependencies, `rmmod` removes, `lsmod` lists (= `/proc/modules`), `modinfo` shows information, `dmesg` shows the kernel log. Module parameters: `module_param`.

### Device drivers
- **Character device**: byte-stream access (serial ports, sensors, `/dev/ttyS0`). **Block device**: fixed-size block access with caching (disks, `/dev/sda`). **Network device**: no file in /dev (`eth0`).
- A device file has a **major number** (selects the driver) and a **minor number** (selects the specific device). `mknod`, `ls -l /dev`.
- A char driver implements `struct file_operations`: `open`, `release`, `read`, `write`, `unlocked_ioctl`, `mmap`. Data exchanged with user space must go through `copy_to_user`/`copy_from_user`.
- The device tree (`.dts` → `.dtb`) describes the hardware to the kernel on ARM instead of hard-coding it.

### Interrupts
- An ISR must be **short**, must not sleep (no mutex, no `kmalloc(GFP_KERNEL)`, no `copy_to_user`), returns nothing to anyone and takes no arbitrary parameters.
- Split into a **top half** (the ISR, urgent work: acknowledge the interrupt, grab the data) and a **bottom half** (deferred work: softirq, tasklet, **workqueue** — workqueues run in process context and may sleep; threaded IRQs).
- Polling vs interrupts: polling is simple but wastes CPU; interrupts are efficient when events are rare. DMA: the device copies data straight into RAM and the CPU only gets an interrupt when it is done.
- Synchronizing with an ISR: disable local interrupts, `spin_lock_irqsave`. Variables shared with an ISR are declared `volatile` (on bare metal).

### Kernel memory
- `kmalloc` (physically contiguous, small, suitable for DMA), `vmalloc` (virtually contiguous only, for large areas), `GFP_KERNEL` (may sleep) vs `GFP_ATOMIC` (never sleeps, used in interrupts).
- The kernel stack is small (usually 8–16 KiB) → never put large arrays on it.

### Embedded and RTOS
- **Real-time**: meeting the **deadline** matters as much as the right result. Hard real-time (a missed deadline = failure, e.g. airbags, ABS brakes), soft real-time (a missed deadline degrades quality, e.g. video). Metrics: latency, jitter, determinism.
- RTOS (FreeRTOS, Zephyr, QNX, AUTOSAR OS): fixed-priority preemptive scheduler, tasks, queues, semaphores, mutexes with priority inheritance, timers. Linux is usually not hard real-time (the PREEMPT_RT patch set has been in mainline since 6.12).
- Bare metal: a `while(1)` loop + interrupts, no OS.
- Memory: Flash (code, constants), RAM (data, bss, stack, heap). Avoid dynamic `malloc` in safety-critical systems (fragmentation, non-deterministic timing).
- **Watchdog timer**: must be "kicked" (refreshed) periodically; if the software hangs, the watchdog resets the system.
- Memory-mapped registers: `#define REG (*(volatile uint32_t *)0x40021000)`.
- Buses: **UART** (asynchronous, 2 wires TX/RX, both sides need the same baud rate), **SPI** (synchronous, full-duplex, 4 wires SCLK/MOSI/MISO/CS, one master and several slaves selected by CS, fast), **I2C** (2 wires SDA/SCL, addressed, multi-master/slave, slower, needs pull-up resistors), **CAN** (2-wire differential bus CAN_H/CAN_L, multi-master, priority by ID — the lowest ID wins arbitration, widely used in cars; CAN FD raises the payload to 64 bytes). LIN (cheap, slow, single master), FlexRay, Automotive Ethernet.
- Automotive: **AUTOSAR** (Classic for real-time ECUs, Adaptive for high-performance computers on POSIX), **MISRA C/C++** (safe coding rules: ban/restrict malloc, recursion, goto, implicit conversions...), **ISO 26262** (functional safety, ASIL levels A–D, D is the highest).

## Quiz

### What kind of kernel is Linux?
- [ ] Microkernel
- [x] Monolithic (with loadable modules at run time)
- [ ] Exokernel
- [ ] Hybrid like Windows NT
> Drivers, filesystems and the scheduler all run in kernel space. Modules are just a way to load code into the kernel at run time; they still run in kernel space.

### Which command loads a module and automatically handles its dependencies?
- [ ] insmod
- [x] modprobe
- [ ] lsmod
- [ ] depmod -r
> insmod only loads the exact .ko file given. depmod builds the dependency table that modprobe uses.

### In a kernel module, what is the logging equivalent of printf?
- [ ] printf
- [x] printk (or pr_info, pr_err...)
- [ ] syslog
- [ ] fprintf(stderr)
> The kernel does not use libc. Read the log with dmesg.

### How do character devices and block devices differ?
- [ ] Character devices are faster
- [x] A character device is accessed as a byte stream; a block device in fixed-size blocks, with buffering and random access
- [ ] Block devices have no file in /dev
- [ ] Character devices are read-only
> Char examples: /dev/ttyS0, /dev/null; block examples: /dev/sda, /dev/mmcblk0.

### What is the major number of a device file for?
- [ ] The device's size
- [x] Identifying the driver that handles the device
- [ ] Access permissions
- [ ] The load order
> The minor number distinguishes the individual devices handled by that driver.

### Which function does a driver use to copy data from the kernel to a user-space buffer?
- [ ] memcpy
- [x] copy_to_user
- [ ] strcpy
- [ ] mmap
> copy_to_user checks that the user address is valid and handles page faults safely. The other direction is copy_from_user.

### Which of these must NOT be done in an interrupt handler (ISR)? (select all that apply)
- [x] Calling a function that may sleep, such as mutex_lock
- [x] Long computations
- [ ] Reading the status register and clearing the interrupt flag
- [x] kmalloc with GFP_KERNEL
> An ISR runs in interrupt context: it must not sleep and must be short. Use GFP_ATOMIC if you really must allocate.

### Why split interrupt handling into a top half and a bottom half?
- [ ] To use two CPUs
- [x] So the ISR (top half) finishes quickly and heavy work is deferred to a bottom half that runs with interrupts re-enabled
- [ ] To avoid volatile
- [ ] Because the kernel requires two functions per interrupt
> Bottom halves: softirq, tasklet, workqueue (may sleep), threaded IRQ.

### Which bottom-half mechanism runs in process context and may sleep?
- [ ] Tasklet
- [ ] Softirq
- [x] Workqueue
- [ ] Top half
> Tasklets and softirqs run in softirq context and must not sleep.

### How do kmalloc and vmalloc differ?
- [ ] vmalloc is faster
- [x] kmalloc returns physically contiguous memory; vmalloc is only virtually contiguous and is used for large areas
- [ ] kmalloc is for user space
- [ ] They are the same
> DMA buffers usually need physically contiguous memory → kmalloc (or dma_alloc_coherent).

### A hard real-time system is one where:
- [ ] Everything runs very fast
- [x] Missing a deadline counts as a system failure
- [ ] No interrupts are used
- [ ] Linux is always used
> E.g. airbag or brake control. Determinism (the worst case) matters, not average speed.

### What is a watchdog timer for?
- [ ] Measuring a task's run time
- [x] Resetting the system automatically if the software hangs and stops "kicking" the watchdog in time
- [ ] Waking the CPU from sleep
- [ ] Synchronizing clocks between ECUs
> The software must refresh the watchdog periodically from the main loop or a supervisor task.

### Which declaration correctly accesses a 32-bit register at address 0x40021000?
- [ ] `uint32_t *REG = 0x40021000;`
- [x] `#define REG (*(volatile uint32_t *)0x40021000)`
- [ ] `const uint32_t REG = 0x40021000;`
- [ ] `static uint32_t REG @ 0x40021000;`
> volatile makes every read/write really access the hardware.

### Which bus uses 2 wires (SDA, SCL) and addresses devices on the bus?
- [ ] SPI
- [x] I2C
- [ ] UART
- [ ] CAN
> SPI uses 4 wires and selects a slave with a CS pin; UART is point-to-point over TX/RX; CAN uses the differential pair CAN_H/CAN_L.

### On a CAN bus, when two nodes transmit at once, which one wins arbitration?
- [ ] The node with the higher ID
- [x] The node with the lower ID (a 0 bit is dominant)
- [ ] The node that started first
- [ ] Random
> Arbitration is non-destructive: the losing node backs off and retries later. Lower ID = higher priority.

### Which are properties of SPI? (select all that apply)
- [x] Synchronous, with a clock generated by the master
- [x] Full-duplex
- [ ] Uses a 7-bit address to select a slave
- [x] Selects a slave with a dedicated CS/SS pin
> 7-bit addressing is an I2C feature.

### What is MISRA C?
- [ ] A C compiler for cars
- [x] A set of C coding rules that improve safety and reliability in embedded/automotive systems
- [ ] A real-time operating system
- [ ] A communication standard between ECUs
> E.g. no dynamic allocation, restricted recursion, no implicit conversions that lose data, every switch needs a default.

### What is the highest ASIL level in ISO 26262?
- [ ] ASIL A
- [ ] QM
- [x] ASIL D
- [ ] ASIL E
> QM (ordinary quality management), then A (lowest) → D (highest, e.g. braking, steering).

### Why do safety-critical embedded systems usually avoid `malloc` after start-up?
- [ ] malloc does not exist in C99
- [x] It fragments memory and its allocation time is not deterministic
- [ ] malloc only works on Linux
- [ ] malloc is slower than new
> Use static memory or pre-allocated memory pools instead.

### What is the usual boot order on an ARM board running Linux?
- [ ] Kernel → U-Boot → ROM → rootfs
- [x] ROM bootloader → U-Boot → kernel + device tree → rootfs/init
- [ ] U-Boot → ROM → init → kernel
- [ ] GRUB → BIOS → kernel → init
> U-Boot loads the kernel (zImage/Image) and the .dtb describing the hardware into RAM, then hands control to the kernel.
