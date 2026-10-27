# 07-kernel-module (chỉ để biết, có thể bỏ qua nếu thiếu thời gian)

Không build bằng `make` ở thư mục gốc. Làm trong thư mục này:

```sh
sudo apt install build-essential linux-headers-$(uname -r)
make
sudo insmod hello.ko            # hoặc: sudo insmod hello.ko count=3
dmesg | tail
lsmod | grep hello
modinfo hello.ko
sudo rmmod hello
dmesg | tail -1
make clean
```

> ⚠️ **Secure Boot**: nếu máy bật Secure Boot, `insmod` có thể bị từ chối
> (`Key was rejected by service`) vì module chưa được ký. Khi đó chỉ cần **build được** `hello.ko` là đủ,
> **không cần** tắt Secure Boot.
