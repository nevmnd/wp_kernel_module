#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */

static long int address = 0, counter = 0;

module_param(address, long, 0);

static int __init test_init(void)
{
	printk(KERN_INFO "Loading test_wp module...\n");
        if (address)
            printk(KERN_INFO "Watchpoint address: 0x%lX\n", address);
        else
            printk(KERN_INFO "Watchpoint address not specified");
        printk(KERN_INFO "Counter address: 0x%ln\n", &counter);
	return 0;
}

static void __exit test_exit(void)
{
	printk(KERN_INFO "Unloading test_wp...\n");

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chestyunin Danil <nevmnd@gmail.com>");
MODULE_DESCRIPTION("watchpoint tester");