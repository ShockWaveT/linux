#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/i2c.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple i2c kernel module.");
MODULE_VERSION("1.0");


#define DEVICE_NAME         "my_i2c_device"
#define CLASS_NAME          "my_i2c_device_class"
#define IOCTL_PRINT_MESSAGE _IO('a', 1)

#define MPU6050_I2C_ADDR 0x68  
#define WHO_AM_I_REG     0x75

static int major;
static struct class *my_class = NULL;
static struct device *my_device = NULL;
static struct i2c_client *mpu6050_client = NULL;

static int dev_open(struct inode *inodep, struct file *filep) 
{
    printk(KERN_INFO "my_device: Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    char reg = WHO_AM_I_REG; 
    char value = 0;
    int ret;

    if (!mpu6050_client) {
        printk(KERN_ERR "my_device: No I2C client available\n");
        return -ENODEV;
    }

    ret = i2c_master_send(mpu6050_client, &reg, 1);
    if (ret < 0) {
        printk(KERN_ERR "my_device: Failed to write to I2C device\n");
        return ret;
    }

    ret = i2c_master_recv(mpu6050_client, &value, 1);
    if (ret < 0) {
        printk(KERN_ERR "my_device: Failed to read from I2C device\n");
        return ret;
    }

    printk(KERN_INFO "my_device: WHO_AM_I register value: 0x%02X\n", value);

    return 0;  // We return 0 bytes read to simulate end-of-file (EOF)
}

static int dev_release(struct inode *inodep, struct file *filep) 
{
    printk(KERN_INFO "my_device: Device closed\n");
    return 0;
}

// This function is called when an ioctl command is issued
static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) 
{
    switch (cmd) {
        case IOCTL_PRINT_MESSAGE:
            printk(KERN_INFO "my_device: ioctl command received\n");
            break;
        default:
            printk(KERN_WARNING "my_device: Unknown ioctl command\n");
            return -ENOTTY;  // Command not supported
    }
    return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl,
};

static int my_i2c_device_probe(struct i2c_client *client)
{
    dev_info(&client->dev, "Probe function: MPU6050 device detected\n");
    mpu6050_client = client;
    return 0;
}

static void my_i2c_device_remove(struct i2c_client *client)
{
    dev_info(&client->dev, "Remove Function: MPU6050 device removed\n");
    mpu6050_client = NULL;
    return ;
}

//Used to export supported devices info to user space. eg. modprobe and udev.
static const struct i2c_device_id mysensor_id[] = {
    { "mpu6050", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, mysensor_id);

static const struct of_device_id mpu6050_of_match[] = {
    { .compatible = "arun,mpu6050" },
    { }
};
MODULE_DEVICE_TABLE(of, mpu6050_of_match);

//used to export supported device info to kernel space.
static struct i2c_driver mysensor_driver = {
    .driver = {
        .name   = "mpu6050",
        .owner  = THIS_MODULE,
	.of_match_table = mpu6050_of_match, // to match with device tree
    },
    .probe  = my_i2c_device_probe,
    .remove = my_i2c_device_remove,
    .id_table = mysensor_id,
};



static int __init hello_init(void)
{
    printk(KERN_INFO "my_device: Initializing the device\n");

    // Allocate a major number for the device
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "my_device: Failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "my_device: Registered with major number %d\n", major);

    // Register the device class
    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "my_device: Failed to register device class\n");
        return PTR_ERR(my_class);
    }
    printk(KERN_INFO "my_device: Device class registered\n");

    // Register the device driver
    my_device = device_create(my_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "my_device: Failed to create the device\n");
        return PTR_ERR(my_device);
    }
    printk(KERN_INFO "my_device: Device class created successfully\n");
    
    return i2c_add_driver(&mysensor_driver);

    return 0;
}

static void __exit hello_exit(void)
{
    device_destroy(my_class, MKDEV(major, 0));
    class_unregister(my_class);
    class_destroy(my_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "my_device: Device unregistered and cleaned up\n");
}

module_init(hello_init);
module_exit(hello_exit);

