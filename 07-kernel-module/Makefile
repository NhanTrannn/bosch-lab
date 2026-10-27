# Makefile riêng cho kernel module (dùng Kbuild của kernel đang chạy)
obj-m += hello.o
KDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
