obj-m+=tmr.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

insmod:
	insmod tmr.ko

rmmod:
	rmmod tmr.ko

check:
	@echo "\e[1;34m=================tmr===================\e[0m"
	@insmod tmr.ko
	@echo "\e[93m"
	cat /sys/class/mydev/timer
	@echo "" ; sleep 1
	cat /sys/class/mydev/timer
	@echo "" ; sleep 2
	cat /sys/class/mydev/timer
	@echo "" ; sleep 3
	cat /sys/class/mydev/timer
	@echo "" ; sleep 1
	cat /sys/class/mydev/timer
	@rmmod tmr
	@echo "\e[0m"

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
