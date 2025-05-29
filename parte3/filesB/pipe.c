#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include "buff_helper.h"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("SC2022/2023");
MODULE_DESCRIPTION("Simple pipe character device driver");
MODULE_VERSION("0:0.1");

/** module functions **/
static int pipe_init(void);
static void pipe_exit(void);

/** device driver functions  **/
static int pipe_open(struct inode *inode, struct file *filp);
static int pipe_release(struct inode *inode, struct file *filp);
ssize_t pipe_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);
ssize_t pipe_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);

/**  global variables for the device driver  **/
static dev_t mydev;
static struct cdev *mycdev;
static struct file_operations myfops = {
    .owner = THIS_MODULE,
	.read = pipe_read,
	.write = pipe_write,
	.open = pipe_open,
	.release = pipe_release
};
/***********/
#define BUFFER_MAX_SIZE 100

char data_space[BUFFER_MAX_SIZE];
circ_buff_t pipe_buffer = { 
    data_space,
    BUFFER_MAX_SIZE,
    0,
    0
};
/************/
static int pipe_open(struct inode *inode, struct file *filp)
{
	// Debug:
	printk(KERN_INFO "pipe_open(): Returning\n");
    return -1;
}
/************/
static int pipe_release(struct inode *inode, struct file *filp)
{
	// Debug:
    printk(KERN_INFO "pipe_release(): Returning\n");
    return 1;
}
/************/
ssize_t pipe_read(struct file *filep, char __user *buff, size_t count, loff_t *offp)
{
    int i, rd_count = 0;
	char rd_data = 0;

	for (i = 0; i<count; i++)
	{
		if (circ_buff_pop(pipe_buffer, &rd_data) != 0)
		{
			printk(KERN_INFO "pipe_read(): No data to read\n");
			break; // no data to read
		}
		if (copy_to_user((void __user *) buff+i, (const void *) &rd_data, 1) != 0)
		{
			printk(KERN_INFO "pipe_read(): Error copying to user\n");
			return -1; // error copying
		}
		rd_count++;
		printk(KERN_INFO "pipe_read(): Read %d characters\n", rd_count);
	}
	if (rd_count == 0)
	{
		// Buffer empty from the start
		printk(KERN_ERR "pipe_read(): FIFO empty\n");
		return 0; // no characters read
	}
	else
		return ((ssize_t) rd_count+1); // return number of bytes read
}
/************/
ssize_t pipe_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp)
{
    // Should write data from user space to circ_buffer.
	int i , wr_count = 0;
	char wr_data;

	for (i = 0; i < count; i++)
	{
		// protection for limited buffer size
		// buffer larger than string to echo (ok, just echo count bytes)
		if (copy_from_user((void *)wr_data,(const void __user *) buff,(unsigned long) count)!= 0)
		{
			printk(KERN_INFO "pipe_write(): Error copying from user\n");
			return -1; // error copying
		}
		if (circ_buf_push(&pipe_buffer, wr_data) != 0)
		{
			printk(KERN_INFO "pipe_write(): String is larger than buffer size... Breaking loop...\n");
			break;
		}
		wr_count++
		printk(KERN_INFO "pipe_write(): Wrote %d characters\n", wr_count);
	}

	if (wr_count == 0) {
		printk(KERN_ERR "pipe_write(): FIFO full\n");
		return -1; // no characters written
	}
	else
		return ((ssize_t) wr_count); // return number of bytes written
}
/**************/
static int pipe_init(void)
{
	int return_of_alloc_chrdev_region = -1;
	int return_of_cdev_add = -1;

	// Register device driver so that:
	// - Driver name is pipe 
	// - Major number is dynamically assigned
	// - Minor number starts from 0
	// - Only one device needs to be managed by the driver

	return_of_alloc_chrdev_region = alloc_chrdev_region(&mydev, 0, 1, "pipe");
	if (return_of_alloc_chrdev_region < 0){
		printk(KERN_ERR "Failed to register pipe device driver\n");
		return return_of_alloc_chrdev_region;
	}

    // Create struture to represent char devices
	mycdev = cdev_alloc();
	if (mycdev == NULL){
		unregister_chrdev_region(mydev, 1);
		printk(KERN_ERR "Failed to allocate cdev structure\n");
		return -1;
	}

	mycdev->owner = THIS_MODULE;
	mycdev->ops = &myfops;

    // Register character device into the kernel
	return_of_cdev_add = cdev_add(mycdev, mydev, 1);
    if (return_of_cdev_add < 0){
        kfree(mycdev);
        unregister_chrdev_region(mydev, 1);
        printk(KERN_ERR "Failed to register character device info\n");
        return return_of_cdev_add;
    }

	// Print "pipe device driver registered with major number X"
	// to the kernel logging buffer so that:
	// - X is the obtained major number during registration
	// - Message printed using the informational log evel
	printk(KERN_INFO "pipe device driver registered with major number X\n");
	return 0;
}
/************/
static void pipe_exit(void)
{
    // deregister character device
	if (mycdev){
		cdev_del(my_cdev);   // Unregister from kernel first
		kfree(my_cdev);      // Then free the memory
	}


	unregister_chrdev_region(mydev, 1); 
    // deregister the device driver's device numbers

	// Print a message to the kernel saying the module has been unregistered
	printk(KERN_INFO "pipe device driver deregistered\n");
}
/************/
module_init(pipe_init);
module_exit(pipe_exit);