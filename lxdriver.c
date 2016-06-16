#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define LX_ACCELL_DRIVER_DEV_NAME "lxaccell"

static int lx_accell_device_open(struct inode *inode, struct file *file)
{
    printk("Opening device\n");
    return 0;
}

static int lx_accell_device_release(struct inode *inode, struct file *file)
{
    printk("Closing device\n");
    return 0;
}

static ssize_t lx_accell_device_read(struct file *file, char __user * buffer,
			   size_t length, loff_t * offset)
{
    printk("Reading device (%d bytes)\n", length);
    return length; /* But we don't actually do anything with the data */
}

static ssize_t lx_accell_device_write(struct file *file, const char __user *buffer,
		       size_t length, loff_t *offset)
{
    printk("Writting device (%d bytes)\n", length);
    return length; /* But we don't actually do anything with the data */
}

static const struct file_operations lx_accell_device_operations = {
    .owner              = THIS_MODULE,
    .read               = lx_accell_device_read,
    .write              = lx_accell_device_write,
    .open               = lx_accell_device_open,
    .release            = lx_accell_device_release
};

struct miscdevice lx_accell_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = LX_ACCELL_DRIVER_DEV_NAME,
    .fops = &lx_accell_device_operations,
};

static int __init lx_driver_init(void)
{
    int error;

    printk("LX Driver init\n");
    error = misc_register(&lx_accell_device);
    if (error) {
        printk("Failed to register device %s\n", LX_ACCELL_DRIVER_DEV_NAME);
        return error;
    }

    printk("Successful register device %s\n", LX_ACCELL_DRIVER_DEV_NAME);
    return 0;
}

static void __exit lx_driver_exit(void)
{
    misc_deregister(&lx_accell_device);
    printk("LX Driver exit\n");
}

module_init(lx_driver_init)
module_exit(lx_driver_exit)

MODULE_DESCRIPTION("Misc driver for LIS3DH accelerometer");
MODULE_AUTHOR("Pascal Jacquemart <p99_pascal@yahoo.fr>");
MODULE_LICENSE("GPL");
