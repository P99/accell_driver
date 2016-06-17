#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>

#include "lis3dh_acc.h"

#define LX_ACCELL_DRIVER_DEV_NAME "lxaccell"

struct lx_accell_private_data {
    struct i2c_client *client;
    int i2c_addr;
    char *test;
};

static void misc_set_drvdata(struct miscdevice *misc, void *pdata)
{
    if (misc && misc->this_device) {
        dev_set_drvdata(misc->this_device, pdata);
    }
};

static void *misc_get_drvdata(struct file *file)
{
    struct miscdevice *misc = file->private_data;
    if (misc && misc->this_device) {
        return dev_get_drvdata(misc->this_device);
    }
    return NULL;
}

static int lx_accell_device_open(struct inode *inode, struct file *file)
{
    struct lx_accell_private_data *pdata = misc_get_drvdata(file);

    printk("Reading device pdata=%p\n", pdata);

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
    struct lx_accell_private_data *pdata = misc_get_drvdata(file);

    printk("Reading device (%d bytes) pdata=%p\n", length, pdata);

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
    .parent = NULL,
    .this_device = NULL
};

static int lx_accell_i2c_read(struct i2c_client *client, u8 *buffer, size_t length)
{
    struct i2c_msg packets[] = {
        {
            .addr = client->addr,
            .flags = 0,
            .len = 1,
            .buf = buffer,
         },
         {
             .addr = client->addr,
             .flags = I2C_M_RD,
             .len = length,
             .buf = buffer,
        }
    };
    return i2c_transfer(client->adapter, packets, 2);
}

static int lx_accell_i2c_write(struct i2c_client *client, u8 *buffer, size_t length)
{
    struct i2c_msg packets[] = {
        {
            .addr = client->addr,
            .flags = 0,
            .len = length,
            .buf = buffer,
         }
    };
    return i2c_transfer(client->adapter, packets, 1);
}

static int lx_accell_i2c_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
    struct lx_accell_private_data *pdata;
    int error;

    pdata = kmalloc(sizeof(struct lx_accell_private_data), GFP_KERNEL);
    if (pdata) {
	pdata->client = client;
        i2c_set_clientdata(client, pdata);
    }

    error = misc_register(&lx_accell_device);
    if (error) {
        printk("Failed to register device %s\n", LX_ACCELL_DRIVER_DEV_NAME);
        return error;
    }

    misc_set_drvdata(&lx_accell_device, pdata);

    return 0;
}

static int lx_accell_i2c_remove(struct i2c_client *client)
{
    void *pdata = i2c_get_clientdata(client);

    misc_deregister(&lx_accell_device);

    printk("Inside i2c_remove\n");
    if (pdata)
        kfree(pdata);

    return 0;
}

static int lx_accell_i2c_detect(struct i2c_client *client, struct i2c_board_info *info)
{
    printk("Inside i2c_detect i2c_board_info: addr=%x, adapter=%p\n", info->addr, client->adapter);

    if (i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        u8 buffer[] = {WHO_AM_I};
        int status = lx_accell_i2c_read(client, buffer, 1);

        printk("i2c_transfer returned status: %d\n", status);

        if (status == 2) {
            printk("Success checking device identity\n");
            if (buffer[0] == WHO_AM_I_OUTPUT) {
                /* Success: Proceed to probe() */
                strcpy(info->type, LX_ACCELL_DRIVER_DEV_NAME);
            }
        } else {
	    printk("Failed to identify the device (read: 0x%x, expected 0x%x)\n", buffer[0], WHO_AM_I_OUTPUT);
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
    I2C_ADDR_PRIMARY,
    I2C_ADDR_SECONDARY
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
    i2c_del_driver(&lx_accell_i2c_driver);
    printk("LX Driver exit\n");
}

module_init(lx_driver_init)
module_exit(lx_driver_exit)

MODULE_DESCRIPTION("Misc driver for LIS3DH accelerometer");
MODULE_AUTHOR("Pascal Jacquemart <p99_pascal@yahoo.fr>");
MODULE_LICENSE("GPL");
