# Kernel Linux và embedded

## Lý thuyết

### Kiến trúc
- **Monolithic kernel** (Linux): scheduler, bộ nhớ, filesystem, driver đều chạy trong kernel space → nhanh, nhưng lỗi driver có thể làm sập cả hệ thống. Linux hỗ trợ **loadable module** để nạp/gỡ lúc chạy.
- **Microkernel** (QNX, seL4): chỉ giữ phần tối thiểu trong kernel, driver/service chạy ở user space → ổn định, an toàn hơn, chậm hơn do IPC.
- User space ↔ kernel space qua system call; kernel không dùng được libc (dùng `printk` thay `printf`, `kmalloc` thay `malloc`).

### Kernel module
```c
#include <linux/module.h>
static int __init hello_init(void) { pr_info("hello\n"); return 0; }
static void __exit hello_exit(void) { pr_info("bye\n"); }
module_init(hello_init); module_exit(hello_exit);
MODULE_LICENSE("GPL");
```
- Build bằng Kbuild (`obj-m += hello.o`, `make -C /lib/modules/$(uname -r)/build M=$PWD modules`) → `hello.ko`.
- `insmod` nạp (không xử lý phụ thuộc), `modprobe` nạp kèm phụ thuộc, `rmmod` gỡ, `lsmod` liệt kê (= `/proc/modules`), `modinfo` xem thông tin, `dmesg` xem log kernel. Tham số module: `module_param`.

### Device driver
- **Character device**: truy cập theo luồng byte (serial, cảm biến, `/dev/ttyS0`). **Block device**: truy cập theo khối, có cache (đĩa, `/dev/sda`). **Network device**: không có file trong /dev (`eth0`).
- File thiết bị có **major number** (chọn driver) và **minor number** (chọn thiết bị cụ thể). `mknod`, `ls -l /dev`.
- Char driver hiện thực `struct file_operations`: `open`, `release`, `read`, `write`, `unlocked_ioctl`, `mmap`. Copy dữ liệu với user space phải qua `copy_to_user`/`copy_from_user`.
- Device tree (`.dts` → `.dtb`) mô tả phần cứng cho kernel trên ARM thay vì hard-code.

### Ngắt (interrupt)
- ISR phải **ngắn**, không được ngủ (không mutex, không `kmalloc(GFP_KERNEL)`, không `copy_to_user`), không trả giá trị cho ai, không nhận tham số tuỳ ý.
- Chia **top half** (ISR, xử lý gấp: ack ngắt, lấy dữ liệu) và **bottom half** (xử lý sau: softirq, tasklet, **workqueue** — workqueue chạy ở process context nên được ngủ; threaded IRQ).
- Polling vs interrupt: polling đơn giản nhưng tốn CPU; interrupt hiệu quả khi sự kiện thưa. DMA: thiết bị chép dữ liệu trực tiếp vào RAM, CPU chỉ nhận ngắt khi xong.
- Đồng bộ với ISR: tắt ngắt cục bộ, `spin_lock_irqsave`. Biến dùng chung với ISR khai báo `volatile` (trong bare-metal).

### Bộ nhớ trong kernel
- `kmalloc` (liên tục cả vật lý, nhỏ, hợp cho DMA), `vmalloc` (liên tục ảo, không liên tục vật lý, vùng lớn), `GFP_KERNEL` (được ngủ) vs `GFP_ATOMIC` (không ngủ, dùng trong ngắt).
- Stack kernel nhỏ (thường 8–16 KiB) → không khai báo mảng lớn trên stack.

### Embedded và RTOS
- **Real-time**: đúng **thời hạn** quan trọng như đúng kết quả. Hard real-time (trễ hạn = hỏng, ví dụ túi khí, phanh ABS), soft real-time (trễ hạn giảm chất lượng, ví dụ video). Thước đo: latency, jitter, tính tất định (determinism).
- RTOS (FreeRTOS, Zephyr, QNX, AUTOSAR OS): scheduler preemptive theo ưu tiên cố định, task, queue, semaphore, mutex có priority inheritance, timer. Linux thường không hard real-time (có bản vá PREEMPT_RT, đã vào mainline từ 6.12).
- Bare-metal: vòng lặp `while(1)` + ngắt, không OS.
- Bộ nhớ: Flash (code, hằng), RAM (data, bss, stack, heap). Hạn chế dùng `malloc` động trong hệ thống an toàn (phân mảnh, không tất định).
- **Watchdog timer**: phải "đá" (reset) định kỳ; nếu phần mềm treo thì watchdog reset hệ thống.
- Thanh ghi memory-mapped: `#define REG (*(volatile uint32_t *)0x40021000)`.
- Giao tiếp: **UART** (bất đồng bộ, 2 dây TX/RX, cần cùng baudrate), **SPI** (đồng bộ, full-duplex, 4 dây SCLK/MOSI/MISO/CS, một master nhiều slave qua CS, nhanh), **I2C** (2 dây SDA/SCL, có địa chỉ, nhiều master/slave, chậm hơn, cần điện trở kéo lên), **CAN** (bus vi sai 2 dây CAN_H/CAN_L, multi-master, ưu tiên theo ID — ID nhỏ thắng arbitration, dùng rộng rãi trong ô tô; CAN FD tăng payload lên 64 byte). LIN (rẻ, chậm, một master), FlexRay, Automotive Ethernet.
- Ô tô: **AUTOSAR** (Classic cho ECU thời gian thực, Adaptive cho máy tính hiệu năng cao trên POSIX), **MISRA C/C++** (bộ quy tắc lập trình an toàn: cấm/hạn chế malloc, đệ quy, goto, chuyển kiểu ngầm...), **ISO 26262** (an toàn chức năng, mức ASIL A–D, D cao nhất).

## Trắc nghiệm

### Linux là kiểu kernel nào?
- [ ] Microkernel
- [x] Monolithic (có hỗ trợ module nạp được lúc chạy)
- [ ] Exokernel
- [ ] Hybrid như Windows NT
> Driver, filesystem, scheduler cùng chạy trong kernel space. Module chỉ là cách nạp code vào kernel lúc chạy, vẫn chạy trong kernel space.

### Lệnh nào nạp module kèm tự động xử lý module phụ thuộc?
- [ ] insmod
- [x] modprobe
- [ ] lsmod
- [ ] depmod -r
> insmod chỉ nạp đúng file .ko được chỉ định. depmod tạo bảng phụ thuộc mà modprobe dùng.

### Trong kernel module, hàm in log tương ứng với printf là?
- [ ] printf
- [x] printk (hoặc pr_info, pr_err...)
- [ ] syslog
- [ ] fprintf(stderr)
> Kernel không dùng libc. Xem log bằng dmesg.

### Khác biệt giữa character device và block device?
- [ ] Character device nhanh hơn
- [x] Character device truy cập theo luồng byte; block device theo khối cố định, có bộ đệm và truy cập ngẫu nhiên
- [ ] Block device không có file trong /dev
- [ ] Character device chỉ đọc
> Ví dụ char: /dev/ttyS0, /dev/null; block: /dev/sda, /dev/mmcblk0.

### Major number của file thiết bị dùng để?
- [ ] Xác định kích thước thiết bị
- [x] Xác định driver xử lý thiết bị
- [ ] Xác định quyền truy cập
- [ ] Xác định thứ tự nạp
> Minor number phân biệt các thiết bị cụ thể cùng dùng driver đó.

### Driver chép dữ liệu từ kernel ra buffer của user space bằng hàm nào?
- [ ] memcpy
- [x] copy_to_user
- [ ] strcpy
- [ ] mmap
> copy_to_user kiểm tra địa chỉ user hợp lệ và xử lý page fault an toàn. Chiều ngược lại là copy_from_user.

### Điều nào KHÔNG được làm trong interrupt handler (ISR)? (chọn tất cả đáp án đúng)
- [x] Gọi hàm có thể ngủ như mutex_lock
- [x] Xử lý tính toán dài
- [ ] Đọc thanh ghi trạng thái và xoá cờ ngắt
- [x] kmalloc với GFP_KERNEL
> ISR chạy trong interrupt context, không được ngủ và phải ngắn. Dùng GFP_ATOMIC nếu thật sự phải cấp phát.

### Vì sao chia xử lý ngắt thành top half và bottom half?
- [ ] Để dùng hai CPU
- [x] Để ISR (top half) xong nhanh, việc nặng được hoãn sang bottom half chạy khi ngắt đã được bật lại
- [ ] Để tránh dùng volatile
- [ ] Vì kernel bắt buộc mỗi ngắt có hai hàm
> Bottom half: softirq, tasklet, workqueue (được ngủ), threaded IRQ.

### Cơ chế bottom half nào chạy trong process context và được phép ngủ?
- [ ] Tasklet
- [ ] Softirq
- [x] Workqueue
- [ ] Top half
> Tasklet và softirq chạy trong softirq context nên không được ngủ.

### Khác biệt giữa kmalloc và vmalloc?
- [ ] vmalloc nhanh hơn
- [x] kmalloc cấp vùng liên tục về vật lý; vmalloc chỉ liên tục về địa chỉ ảo, dùng cho vùng lớn
- [ ] kmalloc dùng cho user space
- [ ] Không khác nhau
> Bộ nhớ cho DMA thường cần liên tục vật lý → kmalloc (hoặc dma_alloc_coherent).

### Hệ thống hard real-time là hệ thống mà:
- [ ] Chạy rất nhanh
- [x] Trễ thời hạn được xem là lỗi hệ thống
- [ ] Không dùng ngắt
- [ ] Luôn dùng Linux
> Ví dụ điều khiển túi khí, phanh. Quan trọng là tính tất định (worst-case), không phải tốc độ trung bình.

### Watchdog timer dùng để làm gì?
- [ ] Đo thời gian chạy của task
- [x] Tự reset hệ thống nếu phần mềm bị treo và không "đá" watchdog kịp thời
- [ ] Đánh thức CPU khỏi chế độ ngủ
- [ ] Đồng bộ đồng hồ giữa các ECU
> Phần mềm phải refresh watchdog định kỳ trong vòng lặp chính/task giám sát.

### Khai báo nào đúng để truy cập thanh ghi 32 bit tại địa chỉ 0x40021000?
- [ ] `uint32_t *REG = 0x40021000;`
- [x] `#define REG (*(volatile uint32_t *)0x40021000)`
- [ ] `const uint32_t REG = 0x40021000;`
- [ ] `static uint32_t REG @ 0x40021000;`
> volatile để mọi lần đọc/ghi đều thực sự truy cập phần cứng.

### Giao thức nào dùng 2 dây (SDA, SCL) và định địa chỉ thiết bị trên bus?
- [ ] SPI
- [x] I2C
- [ ] UART
- [ ] CAN
> SPI dùng 4 dây và chọn slave bằng chân CS; UART 2 dây TX/RX điểm-điểm; CAN dùng cặp vi sai CAN_H/CAN_L.

### Trên bus CAN, khi hai node cùng gửi, node nào thắng arbitration?
- [ ] Node có ID lớn hơn
- [x] Node có ID nhỏ hơn (bit 0 là dominant)
- [ ] Node gửi trước
- [ ] Ngẫu nhiên
> Arbitration không phá huỷ: node thua tự dừng và gửi lại sau. ID nhỏ = ưu tiên cao.

### SPI có đặc điểm nào? (chọn tất cả đáp án đúng)
- [x] Đồng bộ, có clock do master tạo
- [x] Full-duplex
- [ ] Dùng địa chỉ 7 bit để chọn slave
- [x] Chọn slave bằng chân CS/SS riêng
> Định địa chỉ 7 bit là đặc điểm của I2C.

### MISRA C là gì?
- [ ] Một trình biên dịch C cho ô tô
- [x] Bộ quy tắc lập trình C nhằm tăng an toàn và độ tin cậy trong hệ thống nhúng/ô tô
- [ ] Một hệ điều hành thời gian thực
- [ ] Chuẩn giao tiếp giữa các ECU
> Ví dụ: không dùng cấp phát động, hạn chế đệ quy, không chuyển kiểu ngầm làm mất dữ liệu, mỗi switch phải có default.

### Mức ASIL cao nhất trong ISO 26262 là?
- [ ] ASIL A
- [ ] QM
- [x] ASIL D
- [ ] ASIL E
> QM (quản lý chất lượng thông thường), rồi A (thấp) → D (cao nhất, ví dụ phanh, lái).

### Vì sao hệ thống nhúng an toàn thường tránh `malloc` sau khi khởi động?
- [ ] Vì malloc không có trong C99
- [x] Vì gây phân mảnh bộ nhớ và thời gian cấp phát không tất định
- [ ] Vì malloc chỉ dùng được với Linux
- [ ] Vì malloc chậm hơn new
> Thay bằng bộ nhớ tĩnh hoặc memory pool cấp phát trước.

### Thứ tự khởi động thường gặp trên board ARM chạy Linux?
- [ ] Kernel → U-Boot → ROM → rootfs
- [x] ROM bootloader → U-Boot → kernel + device tree → rootfs/init
- [ ] U-Boot → ROM → init → kernel
- [ ] GRUB → BIOS → kernel → init
> U-Boot nạp kernel (zImage/Image) và file .dtb mô tả phần cứng vào RAM rồi chuyển quyền cho kernel.
