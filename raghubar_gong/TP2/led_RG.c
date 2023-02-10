#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int major;
#define NBMAX_LED 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

// static int __init mon_module_init(void)
// {
//    printk(KERN_DEBUG "Hello World <raghubar_gong> !\n");
//    return 0;
// }



// static int btn;
// module_param(btn, int, 0);
// MODULE_PARM_DESC(btn, "numéro du port du bouton");

// static int __init mon_module_init(void)
// {
//     printk(KERN_DEBUG "Hello_world Raghubar_Gong !\n");
//     printk(KERN_DEBUG "btn=%d !\n", btn);
//     return 0;
// }




static int 
open_led_XY(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open()\n");
    return 0;
}

static ssize_t 
read_led_XY(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read()\n");
    return count;
}

static ssize_t 
write_led_XY(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "write()\n");
    return count;
}

static int 
release_led_XY(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close()\n");
    return 0;
}

struct file_operations fops_led =
{
    .open       = open_led_XY,
    .read       = read_led_XY,
    .write      = write_led_XY,
    .release    = release_led_XY 
};

static int leds[NBMAX_LED];
static int nbled;
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

static int __init mon_module_init(void)
{
    int i;
    major = register_chrdev(0, "led_RG", &fops_led); // 0 est le numéro majeur qu'on laisse choisir par linux
    printk(KERN_DEBUG "Hello World R&G!\n");
    for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "LED_RG %d = %d\n", i, leds[i]);
    return 0;
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "led_RG");
   printk(KERN_DEBUG "Goodbye World <raghubar_gong> !\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);

