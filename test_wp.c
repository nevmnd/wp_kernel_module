#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/delay.h>

static short int use_sys = 0, counter = 0, i = 0;

module_param(use_sys, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

// function for many symbol data enter
static ssize_t show_value(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", counter);
}

// register function to attribute
static struct kobj_attribute test_attribute = __ATTR(counter, 0664, show_value, NULL);

// put attribute to attribute group
static struct attribute *attrs[] = {
    &test_attribute.attr,
    NULL,   /* NULL terminate the list*/
};
static struct attribute_group  reg_attr_group = {
    .attrs = attrs,
};

static struct kobject *test_kobj;

static int __init test_init(void)
{
	printk(KERN_INFO "Loading test_wp module...\n");
        if (use_sys)
            printk(KERN_INFO "Flag use_sys is set. Variable address will be stored in /sys/kernel/wp_addr\n");
        test_kobj = kobject_create_and_add("wp_addr", kernel_kobj);
        if (!test_kobj)
            return -ENOMEM;
        if(sysfs_create_group(test_kobj, &reg_attr_group)){
            kobject_put(test_kobj);
            return -ENOMEM;
        }
        //probably %p
        printk(KERN_INFO "Counter address: 0x%px\n", &counter);
        while (counter < 3) {
            counter++;
            msleep (10000);
            printk(KERN_INFO "Counter is: %d\n", counter);
            msleep (10000);
        };
	return 0;
}

static void __exit test_exit(void)
{
	printk(KERN_INFO "Unloading test_wp...\n");
        kobject_put(test_kobj);

}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chestyunin Danil <nevmnd@gmail.com>");
MODULE_DESCRIPTION("watchpoint tester");