#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/moduleparam.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Simiklit");
MODULE_DESCRIPTION("My simple 'tolower' driver test (which uses a sysfs)");
MODULE_VERSION("0.1");

#define MINIMUM(x, y) ((x) < (y) ? (x) : (y))


static ssize_t mystore(struct class *cl, struct class_attribute *attr,
					const char *buffer, size_t size);
static ssize_t myshow(struct class *cl, struct class_attribute *attr,
					char *buffer);

static struct class_attribute class_attr_tolower =
		__ATTR(tolower,  0664, myshow, mystore);
static struct class_attribute class_attr_stats =
		__ATTR(stats,  0444, myshow, mystore);

static struct class *class_entry;
static size_t data_size;
static char data_buffer[PAGE_SIZE + 1];

static struct {
	unsigned int nprocessed;
	unsigned int nconverted;
	unsigned int nstore;
	unsigned int nshow;
} total_stats;

static int mytolower(int input)
{
	if (input >= 'A' && input <= 'Z') {
		input = 'a' + (input - 'A');
		total_stats.nconverted++;
	}
	total_stats.nprocessed++;
	return input;
}

static ssize_t mystore(struct class *cl, struct class_attribute *attr,
					const char *buffer, size_t size)
{
	size_t i;

	++total_stats.nstore;
	data_size = MINIMUM(size, PAGE_SIZE);
	memcpy(data_buffer, buffer, data_size);


	for (i = 0u; i < data_size; i++)
		data_buffer[i] = mytolower(data_buffer[i]);

	pr_info("write data(%s) size(%u)\n",
				data_buffer, data_size);

	total_stats.nstore++;
	return data_size;
}

static ssize_t myshow(struct class *cl, struct class_attribute *attr,
					char *buffer)
{
	size_t towrite;

	if (attr == &class_attr_stats) {
		char message[1000];

		towrite = sprintf(message, "stats:\n"
			"nprocessed=%u\n"
			"nconverted=%u\n"
			"nstore=%u\n"
			"nshow=%u\n",
			total_stats.nprocessed,
			total_stats.nconverted,
			total_stats.nstore,
			total_stats.nshow);
		memcpy(buffer, message, towrite);
		pr_info("stats sent(%u)", towrite);
	} else if (attr == &class_attr_tolower) {
		towrite = MINIMUM(PAGE_SIZE, data_size);
		memcpy(buffer, data_buffer, towrite);
		pr_info("read data sent(%u)", towrite);
	} else
		towrite = 0;

	total_stats.nshow++;
	return towrite;
}

static ssize_t kinit(void)
{
	int oval = 0;

	class_entry = class_create(THIS_MODULE, "mydev");

	if (class_entry == NULL) {
		pr_info("error: Unable to create 'mydev' class entry in sysfs!!!");
		return -ENOMEM;
	}

	oval = class_create_file(class_entry, &class_attr_tolower);
	if (oval) {
		pr_info("error: Unable to create class file!!!");
		class_destroy(class_entry);
		return oval;
	}

	oval = class_create_file(class_entry, &class_attr_stats);
	if (oval) {
		pr_info("error: Unable to create class file!!!");
		class_remove_file(class_entry, &class_attr_tolower);
		class_destroy(class_entry);
		return oval;
	}

	pr_info("[tolower] driver started");
	return oval;
}

static void kexit(void)
{
	class_remove_file(class_entry, &class_attr_stats);
	class_remove_file(class_entry, &class_attr_tolower);
	class_destroy(class_entry);
	pr_info("[tolower] driver exit");
}

module_init(kinit)
module_exit(kexit)
