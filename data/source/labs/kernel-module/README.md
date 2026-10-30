# kernel-module (for reference — skip it if you are short on time)

Do not build it with `make` at the top level. Work in this directory:

```sh
sudo apt install build-essential linux-headers-$(uname -r)
make
sudo insmod hello.ko            # or: sudo insmod hello.ko count=3
dmesg | tail
lsmod | grep hello
modinfo hello.ko
sudo rmmod hello
dmesg | tail -1
make clean
```

> ⚠️ **Secure Boot**: if Secure Boot is enabled, `insmod` may be refused
> (`Key was rejected by service`) because the module is not signed. Then simply **building** `hello.ko` is enough;
> you do **not** need to disable Secure Boot.
