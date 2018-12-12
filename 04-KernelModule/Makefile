obj-m+=kmain.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

check:
	insmod kmain.ko init=0
	lsmod | grep 'kmain'
	rmmod kmain

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
