#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/moduleparam.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Simiklit");
MODULE_DESCRIPTION("My simple 'toupper' driver test");
MODULE_VERSION("0.1");

#define MINIMUM(x, y) ((x) < (y) ? (x) : (y))

static struct proc_dir_entry *toupper_ent;
static struct proc_dir_entry *stats_ent;
static size_t read_offset;
static size_t data_size;
static char data_buffer[PAGE_SIZE + 1];

static struct {
	unsigned int nprocessed;
	unsigned int nconverted;
	unsigned int nread;
	unsigned int nwrite;
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

static int open_callback(struct inode *nd, struct file *f)
{
	//to be able to read again, again ...
	read_offset = 0;
	pr_info("Open procefs entry");
	return 0;
}

static ssize_t write_callback(struct file *f, const char *ubuf,
						size_t count, loff_t *ppos)
{
	size_t i;

	read_offset = 0;
	data_size = MINIMUM(count, PAGE_SIZE);
	if (copy_from_user(data_buffer, ubuf, data_size)) {
		pr_info("Error: Unable copy from user");
		return -EFAULT;
	}

	for (i = 0u; i < data_size; i++)
		data_buffer[i] = mytoupper(data_buffer[i]);

	pr_info("write data(%s) size(%u)\n", data_buffer, data_size);
	total_stats.nwrite++;
	return data_size;
}

static ssize_t read_callback(struct file *f, char *buf,
						size_t count, loff_t *offp)
{
	size_t towrite = MINIMUM(count, data_size - read_offset);

	if (copy_to_user(buf, data_buffer + read_offset, towrite)) {
		pr_info("Error: Unable copy to user");
		return -EFAULT;
	}
	read_offset += towrite;

	pr_info("read data sent(%u)", towrite);
	total_stats.nread++;
	return towrite;
}

static ssize_t read_stats_callback(struct file *f, char *buf,
						size_t count, loff_t *offp)
{
	static size_t sent;
	size_t towrite;
	char message[1000];

	if (!sent) {
		towrite = sprintf(message, "stats:\n"
			"nprocessed=%u\n"
			"nconverted=%u\n"
			"nread=%u\n"
			"nwrite=%u\n",
			total_stats.nprocessed,
			total_stats.nconverted,
			total_stats.nread,
			total_stats.nwrite);

		sent = towrite;
		if (copy_to_user(buf, message, towrite)) {
			pr_info("Error: Unable copy a msg to user");
			return -EFAULT;
		}
		read_offset += towrite;
	} else
		sent = 0;

	pr_info("read data sent(%u)", sent);
	return sent;
}

static ssize_t kinit(void)
{
	static const struct file_operations toupper_ops = {
		.owner = THIS_MODULE,
		.read = read_callback,
		.write = write_callback,
		.open = open_callback,
	};
	static const struct file_operations stats_ops = {
		.owner = THIS_MODULE,
		.read = read_stats_callback,
	};

	toupper_ent = proc_create("mydev", 0664, NULL, &toupper_ops);
	if (toupper_ent == NULL) {
		pr_info("error: Unable to create 'mydev' entry in proc_fs!!!");
		return -ENOMEM;
	}

	stats_ent = proc_create("mydev_stats", 0664, NULL, &stats_ops);
	if (stats_ent == NULL) {
		pr_info("error: Unable to create 'mydev' entry in proc_fs!!!");
		proc_remove(toupper_ent);
		return -ENOMEM;
	}

	pr_info("Driver started");
	return 0;
}

static void kexit(void)
{
	proc_remove(stats_ent);
	proc_remove(toupper_ent);
	pr_info("[toupper] 'toupper' driver exit");
}

module_init(kinit)
module_exit(kexit)
