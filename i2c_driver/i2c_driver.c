#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple hello world kernel module.");
MODULE_VERSION("1.0");


#define DEVICE_NAME "my_i2c_device"
#define CLASS_NAME  "my_i2c_device_class"

static int major;
static struct class *my_class = NULL;
static struct device *my_device = NULL;

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "my_device: Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "my_device: Device file read\n");
    return 0;  // We return 0 bytes read to simulate end-of-file (EOF)
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "my_device: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
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
    my_class = class_create(THIS_MODULE, CLASS_NAME);
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

