#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/moduleparam.h>
#include <linux/uaccess.h>
#include "strtab.h"
//#include "strtab.c"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Simiklit");
MODULE_DESCRIPTION("My simple test toupper2");
MODULE_VERSION("0.1");

#define MINIMUM(x, y) ((x) < (y) ? (x) : (y))


static ssize_t mystore(struct class *cl, struct class_attribute *attr,
					const char *buffer, size_t size);
static ssize_t myshow(struct class *cl, struct class_attribute *attr,
					char *buffer);

static struct class_attribute class_attr_toupper =
		__ATTR(toupper,  0664, myshow, mystore);
static struct class_attribute class_attr_stats =
		__ATTR(stats,  0444, myshow, mystore);

static struct class *class_entry;
static struct strtab *stab;

static struct {
	unsigned int nprocessed;
	unsigned int nconverted;
	unsigned int nstore;
	unsigned int nshow;
} total_stats;


static int mytoupper(int input)
{
	if (input >= 'a' && input <= 'z') {
		input = 'A' + (input - 'a');
		total_stats.nconverted++;
	}
	total_stats.nprocessed++;
	return input;
}

static ssize_t mystore(struct class *cl, struct class_attribute *attr,
					const char *buffer, size_t size)
{
	size_t i;
	char *str;

	str = strtab_pushback(stab, size + 1);
	if(!str) {
		pr_err("error: strtab can't allocate memory!!!");
		return -ENOMEM;
	}

	memcpy(str, buffer, size);
	str[size] = 0;

	for (i = 0u; i < size; i++)
		str[i] = mytoupper(str[i]);

	pr_info("write data(%s) size(%u)\n",
				str, size);

	total_stats.nstore++;
	return size;
}

static ssize_t myshow(struct class *cl, struct class_attribute *attr,
					char *buffer)
{
	size_t wrt;

	if (attr == &class_attr_stats) {
		wrt = scnprintf(buffer, PAGE_SIZE,
			"stats:\n"
			"nprocessed=%u\n"
			"nconverted=%u\n"
			"nstore=%u\n"
			"nshow=%u\n",
			total_stats.nprocessed,
			total_stats.nconverted,
			total_stats.nstore,
			total_stats.nshow);
		pr_info("stats sent(%u)", wrt);

	} else if (attr == &class_attr_toupper) {
		size_t i, off, ln;
		size_t count = strtab_size(stab);

		for (i = 0, off = 0; i < count && (off + 2) < PAGE_SIZE; i++) {
			const char *val = strtab_get(stab, i);
			if(val == NULL) {
				pr_err("can't get %u element", (unsigned int)i);
				continue;
			}

			ln = strlen(val) + 2;
			if(ln < (PAGE_SIZE - (off + 2)))
			{
				memcpy(buffer + off, val, ln);
				buffer[off + ln + 0] = '\n';
				buffer[off + ln + 1] = '\0';
				off += ln;
			}
			else
				break;
		}
		wrt = off;
		pr_info("read data sent(%u)", wrt);
	} else
		wrt = 0;

	total_stats.nshow++;
	return wrt;
}

static ssize_t kinit(void)
{
	int oval = 0;

	class_entry = class_create(THIS_MODULE, "mydev");
	if (class_entry == NULL) {
		pr_err("error: Unable to create 'mydev' class entry in sysfs!!!");
		goto err_class;
	}

	oval = class_create_file(class_entry, &class_attr_toupper);
	if (oval) {
		pr_err("error: Unable to create class file!!!");
		goto err_toupper;
	}

	oval = class_create_file(class_entry, &class_attr_stats);
	if (oval) {
		pr_err("error: Unable to create class file!!!");
		goto err_stats;
	}

	stab = strtab_create();
	if (stab == NULL) {
		pr_err("error: Unable to create strtab!!!");
		oval = -ENOMEM;
		goto err_strtab;
	}
	pr_err("[toupper] driver started");
	return oval;

err_strtab:
	class_remove_file(class_entry, &class_attr_stats);
err_stats:
	class_remove_file(class_entry, &class_attr_toupper);
err_toupper:
	class_destroy(class_entry);
err_class:
	return oval;
}

static void kexit(void)
{
	strtab_destroy(stab);
	class_remove_file(class_entry, &class_attr_stats);
	class_remove_file(class_entry, &class_attr_toupper);
	class_destroy(class_entry);
	pr_err("[toupper] driver exit");
}

module_init(kinit)
module_exit(kexit)