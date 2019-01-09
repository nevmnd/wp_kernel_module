#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static u32 address = 0, old_address = 0;
struct perf_event * __percpu *wp_hbp;
static struct task_struct *addr_thread;
static char param_buf[128];
static unsigned short int reg_hbp = 0;


module_param_string(address, param_buf, sizeof(param_buf), 0664);


static ssize_t store_value(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    kstrtou32(buf, 0, &address);
    printk(KERN_INFO "Watchpoint address from /sys/kernel/watchpoint/address: 0x%lx\n", address);
    return count;
}

static struct kobj_attribute wp_attribute = __ATTR(address, 0664, NULL, store_value);

static struct attribute *attrs[] = {
    &wp_attribute.attr,
    NULL,
};
static struct attribute_group  reg_attr_group = {
    .attrs = attrs,
};

static struct kobject *wp_kobj;

static void wp_hbp_handler(struct perf_event *bp,
			       struct perf_sample_data *data,
			       struct pt_regs *regs)
{
	printk(KERN_INFO "0x%lx value is changed\n", address);
	//dump_stack();
}

static int hbp_init(void)
{
    struct perf_event_attr attr;
    
    hw_breakpoint_init(&attr);
    attr.bp_addr = address;
    attr.bp_len = HW_BREAKPOINT_LEN_2;
    attr.bp_type = HW_BREAKPOINT_W | HW_BREAKPOINT_R;
    
    wp_hbp = register_wide_hw_breakpoint(&attr, wp_hbp_handler, NULL);
    if (IS_ERR((void __force *)wp_hbp)) {
        printk(KERN_INFO "Breakpoint registration failed. Wrong address or unable to register\n");
	return 1;
    }
    
    printk(KERN_INFO "Breakpoint for 0x%lx access installed\n", address);
    return 0;
 
}

static int addr_handler(void *data)
{
    while(!kthread_should_stop()) {
        if (old_address != address) {
            old_address = address;
            printk(KERN_INFO "Watchpoint address: 0x%lx\n", address);
            if (reg_hbp) {
                unregister_wide_hw_breakpoint(wp_hbp);
                reg_hbp = 0;
            } 
            if (!hbp_init())
                reg_hbp = 1;
        }
        msleep (3000);
    }
    return 0;
}

static int __init wp_init(void)
{
    int err;
    
    printk(KERN_INFO "Loading watchpoint module...\n");
    kstrtou32(param_buf, 0, &address);
    printk(KERN_INFO "Watchpoint address: 0x%lx\n", address);

    wp_kobj = kobject_create_and_add("watchpoint", kernel_kobj);
    if (!wp_kobj)
        return -ENOMEM;
    if(sysfs_create_group(wp_kobj, &reg_attr_group)){
        kobject_put(wp_kobj);
        return -ENOMEM;
    }
    
    addr_thread = kthread_create(addr_handler, NULL, "address_handler");
    if(IS_ERR(addr_thread)){ 
        printk("Unable to start kernel thread\n");
        err = PTR_ERR(addr_thread);
        addr_thread = NULL;
        return err;
    }
    wake_up_process(addr_thread);
    return 0;
}

static void __exit wp_exit(void)
{
	kobject_put(wp_kobj);
        if (reg_hbp){
            unregister_wide_hw_breakpoint(wp_hbp);
            printk(KERN_INFO "Breakpoint for 0x%lx uninstalled\n", address);
        }
        kthread_stop(addr_thread);
	printk(KERN_INFO "Unloading watchpoint...\n");
}

module_init(wp_init);
module_exit(wp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chestyunin Danil <danil.chest@gmail.com>");
MODULE_DESCRIPTION("Backtrace for custom address watchpoint module");