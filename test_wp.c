#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/delay.h>
#include <linux/kthread.h>

static unsigned int counter = 0;
static struct task_struct *test_thread;

static int test_handler(void *data)
{
    while (!kthread_should_stop()) {
        counter++;
        msleep (20000);
        printk(KERN_INFO "Counter is: %u\n", counter);
    };
    return 0;
}


static int __init test_init(void)
{
    int err;
	
    printk(KERN_INFO "Loading test_wp module...\n");
    printk(KERN_INFO "Counter address: 0x%px\n", &counter);

    test_thread = kthread_create(test_handler, NULL, "test_handler");
    if(IS_ERR(test_thread)){ 
        printk("Unable to start kernel thread\n");
        err = PTR_ERR(test_thread);
        test_thread = NULL;
        return err;
        }
    wake_up_process(test_thread);

    return 0;
}

static void __exit test_exit(void)
{
        kthread_stop(test_thread);
	printk(KERN_INFO "Unloading test_wp...\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chestyunin Danil <danil.chest@gmail.com>");
MODULE_DESCRIPTION("Watchpoint module tester");