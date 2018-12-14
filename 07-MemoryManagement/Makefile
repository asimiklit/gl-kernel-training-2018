obj-m += memmod.o
memmod-y += toupper2.o strtab.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

insmod:
	insmod memmod.ko

rmmod:
	rmmod memmod.ko

check:
	@echo "\e[1;34m=================memmod===================\e[0m"
	@insmod memmod.ko
	bash -c 'echo "TEST STRING" > /sys/class/mydev/toupper'
	cat /sys/class/mydev/toupper
	@echo "\e[93m"
	cat /sys/class/mydev/stats
	@rmmod memmod
	@echo "\e[0m"

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
