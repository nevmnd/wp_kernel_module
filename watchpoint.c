#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */

static long int address = 0;

module_param(address, long, 0);

static int __init wp_init(void)
{
	printk(KERN_INFO "Loading watchpoint module...\n");
	printk(KERN_INFO "Watchpoint address: 0x%lX\n", address);
	return 0;
}

static void __exit wp_exit(void)
{
	printk(KERN_INFO "Unloading watchpoint...\n");

}

module_init(wp_init);
module_exit(wp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chestyunin Danil <nevmnd@gmail.com>");
MODULE_DESCRIPTION("custom address watchpoint backtrace");