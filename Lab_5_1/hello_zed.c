#include <linux/module.h>
static int hello_init(void)
{
	printk(KERN_ALERT "hellow zedboard!!! \n");
}
static void hello_exit(void)
{
	printk(KERN_ALERT "Close Module!!! \n");
}
module_init(hello_init);
module_exit(hello_exit);
