#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/moduleparam.h>
#include <linux/uaccess.h>
#include <linux/time.h>
#include <asm/div64.h>

static ssize_t myshow(struct class *cl, struct class_attribute *attr,
					char *buffer);


static struct class *class_entry;
static struct class_attribute class_attr_timer =
		__ATTR(timer,  0444, myshow, NULL);
static struct timer_list print_time_timer;
static int stop;

static uint64_t last_access_timestamp;

void it_time_to_print(unsigned long tm)
{
	uint64_t currentt;
	struct timespec tval;

	getnstimeofday(&tval);
	currentt = (uint64_t)timespec_to_ns(&tval);
	//because of a warning WARNING: "__aeabi_ldivmod" ... undefined!
	//Looks like one way to devide int64 under orange pi
	//it is a usage of this function 'do_div'
	do_div(currentt, 1000000LL);
	pr_info("It is time for a message %llu)))", currentt);
	print_time_timer.expires = jiffies + (1 * HZ);
	print_time_timer.data = 0;
	if(!stop)//some memory barier is needed here I think and below
		add_timer(&print_time_timer);
}

static ssize_t myshow(struct class *cl, struct class_attribute *attr,
					char *buffer)
{
	size_t towrite;
	struct timespec tval;
	uint64_t currentt;

	getnstimeofday(&tval);
	currentt = (uint64_t)timespec_to_ns(&tval);
	//because of a warning WARNING: "__aeabi_ldivmod" ... undefined!
	//Looks like one way to devide int64 under orange pi
	//it is a usage of this function 'do_div'
	do_div(currentt, 1000000LL);

	towrite = sprintf(buffer,
		"last access time diff: %llums\n"
		"last access time: %llums\n"
		"total time since 1970: %llums\n",
		(currentt - last_access_timestamp),
		last_access_timestamp,
		currentt);

	last_access_timestamp = currentt;

	return towrite;
}

static ssize_t kinit(void)
{
	int oval = 0;

	class_entry = class_create(THIS_MODULE, "mydev");

	if (class_entry == NULL) {
		pr_info("error: unable to create 'mydev' class entry in sysfs!!!");
		return -ENOMEM;
	}

	oval = class_create_file(class_entry, &class_attr_timer);
	if (oval) {
		pr_info("error: unable to create class file!!!");
		class_destroy(class_entry);
		return oval;
	}

	init_timer(&print_time_timer);
	print_time_timer.function = it_time_to_print;
	print_time_timer.expires = jiffies + (1 * HZ);
	print_time_timer.data = 0;
	add_timer(&print_time_timer);

	pr_info("[tmr] driver started");
	return oval;
}

static void kexit(void)
{
	//some memory barier is needed here I think and above
	stop = 1;
	class_remove_file(class_entry, &class_attr_timer);
	class_destroy(class_entry);
	del_timer_sync(&print_time_timer);
	pr_info("[tmr] driver exit");
}

module_init(kinit)
module_exit(kexit)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Simiklit");
MODULE_DESCRIPTION("My simple 'tmr' driver test (which uses a sysfs)");
MODULE_VERSION("0.1");
