obj-m+=toupper.o tolower.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

insmod:
	insmod tolower.ko
	insmod toupper.ko

rmmod:
	rmmod tolower.ko
	rmmod toupper.ko

check:
	@echo "\e[1;34m=================toupper===================\e[0m"
	@insmod toupper.ko
	echo "test string" > /proc/mydev
	cat /proc/mydev
	@echo "\e[93m"
	cat /proc/mydev_stats
	@rmmod toupper
	@echo "\e[0m"
	@echo "\e[1;34m=================tolower===================\e[0m"
	@insmod tolower.ko
	bash -c 'echo "TEST STRING" > /sys/class/mydev/tolower'
	cat /sys/class/mydev/tolower
	@echo "\e[93m"
	cat /sys/class/mydev/stats
	@rmmod tolower
	@echo "\e[0m"

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
