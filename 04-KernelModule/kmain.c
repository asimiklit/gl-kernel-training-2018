#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Simiklit");
MODULE_DESCRIPTION("My simple driver test");
MODULE_VERSION("0.1");


static int __init hello_init(void) {
   printk(KERN_DEBUG "Hello everyone!!!!");
   return 0;
}

static void __exit hello_exit(void) {
   printk(KERN_DEBUG "My own driver test finished!");
}

module_init(hello_init);
module_exit(hello_exit);
