#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */

static int address = 0;

module_param(address, int, 0);

static ssize_t store_value(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    kstrtoint(buf, 10, &address);
    printk(KERN_INFO "Watchpoint address from /sys/kernel/wp_addr/counter: 0x%u\n", address);
    return count;
}

// register function to attribute
static struct kobj_attribute wp_attribute = __ATTR(counter, 0664, NULL, store_value);

// put attribute to attribute group
static struct attribute *attrs[] = {
    &wp_attribute.attr,
    NULL,   /* NULL terminate the list*/
};
static struct attribute_group  reg_attr_group = {
    .attrs = attrs,
};

static struct kobject *wp_kobj;

static int __init wp_init(void)
{
	printk(KERN_INFO "Loading watchpoint module...\n");
	if (address)
            printk(KERN_INFO "Watchpoint address from module params: 0x%u\n", address);
        else {
            wp_kobj = kobject_create_and_add("wp_addr", kernel_kobj);
            if (!wp_kobj)
                return -ENOMEM;
            if(sysfs_create_group(wp_kobj, &reg_attr_group)){
                kobject_put(wp_kobj);
                return -ENOMEM;
            }
        }
	return 0;
}

static void __exit wp_exit(void)
{
	kobject_put(wp_kobj);
	printk(KERN_INFO "Unloading watchpoint...\n");

}

module_init(wp_init);
module_exit(wp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chestyunin Danil <nevmnd@gmail.com>");
MODULE_DESCRIPTION("custom address watchpoint backtrace");