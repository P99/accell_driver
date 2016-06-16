#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>

#define LX_ACCELL_DRIVER_DEV_NAME "lxaccell"

#define LIS3DH_ID 0x33
#define LIS3DH_ADDR_PRIMARY 0x18
#define LIS3DH_ADDR_SECONDARY 0x19

/* LIS3DH registers */
#define WHO_AM_I 0x0F


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

static int lx_accell_i2c_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
    printk("Inside i2c_probe (adapter=%p)\n", client->adapter);
    return 0;
}

static int lx_accell_i2c_remove(struct i2c_client *client)
{
    printk("Inside i2c_remove\n");
    return 0;
}

static int lx_accell_i2c_detect(struct i2c_client *client, struct i2c_board_info *info)
{
    int status = -1;
    u8 buf[] = {WHO_AM_I};

    struct i2c_msg msgs[] = {
        {
            .addr = info->addr,
            .flags = 0,
            .len = 1,
            .buf = buf,
         },
         {
             .addr = info->addr,
             .flags = I2C_M_RD,
             .len = 1,
             .buf = buf,
        },
    };
    printk("Inside i2c_detect i2c_board_info: addr=%x, adapter=%p\n", info->addr, client->adapter);

    if (i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        status = i2c_transfer(client->adapter, msgs, 2);
        printk("i2c_transfer returned status: %d\n", status);

        if (status == 2) {
            printk("Success reading: 0x%x (expecting 0x%x)\n", buf[0], LIS3DH_ID);
            if (buf[0] == LIS3DH_ID) {
                /* Success: Proceed to probe() */
                strcpy(info->type, LX_ACCELL_DRIVER_DEV_NAME);
            }
        }
    } else {
        printk("I2C feature not enabled?\n");
    }


    return 0;
}

static const struct i2c_device_id lx_accell_i2c_driver_id[] = {
    { LX_ACCELL_DRIVER_DEV_NAME, 0},
    { }
};

MODULE_DEVICE_TABLE(i2c, lx_accell_i2c_driver_id);

const unsigned short lx_accell_i2c_address_list[] = {
    LIS3DH_ADDR_PRIMARY,
    LIS3DH_ADDR_SECONDARY
};

static struct i2c_driver lx_accell_i2c_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = LX_ACCELL_DRIVER_DEV_NAME,
    },
    .id_table = lx_accell_i2c_driver_id,
    .probe = lx_accell_i2c_probe,
    .remove = lx_accell_i2c_remove,

    /* For auto-detect*/
    .class		= I2C_CLASS_HWMON,
    .detect		= lx_accell_i2c_detect,
    .address_list	= lx_accell_i2c_address_list
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

    error = i2c_add_driver(&lx_accell_i2c_driver);
    if (error) {
        printk("Failed to register i2c driver for device %s\n", LX_ACCELL_DRIVER_DEV_NAME);
        return error;
    }

    printk("Successful register device %s\n", LX_ACCELL_DRIVER_DEV_NAME);
    return 0;
}

static void __exit lx_driver_exit(void)
{
    misc_deregister(&lx_accell_device);
    i2c_del_driver(&lx_accell_i2c_driver);
    printk("LX Driver exit\n");
}

module_init(lx_driver_init)
module_exit(lx_driver_exit)

MODULE_DESCRIPTION("Misc driver for LIS3DH accelerometer");
MODULE_AUTHOR("Pascal Jacquemart <p99_pascal@yahoo.fr>");
MODULE_LICENSE("GPL");
