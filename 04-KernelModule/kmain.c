#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Simiklit");
MODULE_DESCRIPTION("My simple driver test");
MODULE_VERSION("0.1");

static char *init = "Undefined";
module_param(init, charp, 0644);

static int mytest_init(void)
{
	int oval;

	if (strcmp(init, "Error") == 0) {
		oval = -EINVAL;
		printk(KERN_DEBUG "Error");
	} else if (strcmp(init, "OK") == 0) {
		oval = 0;
		printk(KERN_DEBUG "OK");
	} else if (strcmp(init, "Undefined") == 0) {
		oval = -EINVAL;
		printk(KERN_DEBUG "Undefined");
	} else {
		oval = -EINVAL;
		printk(KERN_DEBUG "Error: Unexpected paramter!!!");
	}
	return oval;
}

static void mytest_exit(void)
{
	printk(KERN_DEBUG "My simple test finished!");
}


module_init(mytest_init);
module_exit(mytest_exit);
