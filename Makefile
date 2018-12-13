obj-m += watchpoint.o
obj-m += test_wp.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	sudo dmesg -C
	sudo insmod chrdev.ko address=0x12345
	sudo rmmod chrdev.ko
	dmesg