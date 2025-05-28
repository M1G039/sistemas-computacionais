/********************************
* Device driver for an echo device
*********************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Me");
MODULE_DESCRIPTION("Simple echo character device driver");
MODULE_VERSION("0:0.1");

/** device driver functions  **/
static int echo_open(struct inode *inode, struct file *filp);
static int echo_release(struct inode *inode, struct file *filp);
ssize_t echo_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);
ssize_t echo_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);

/** module functions **/
static int echo_init(void);
static void echo_exit(void);

/**  globas variables for the device driver  **/
static dev_t mydev;

static struct cdev *mycdev;
static struct file_operations myfops = {
    .owner = THIS_MODULE,
	.read = echo_read,
	.write = echo_write,
	.open = echo_open,
	.release = echo_release
	.llseek = no_llseek   //(Exercise 5): Make this a non-seekable device
};

// Here, the driver should define some variables for storing the number of echoed characters 
// since the last time a read instruction was called...

static int echo_open(struct inode *inode, struct file *filp)
{
    //(Exercise 5): Properly link file's private_data
	filp->private_data = mycdev;

    //(Exercise 5): Make this a non-seekable device
	nonseekable_open(inode, filp);

	// for debug:
	printk(KERN_INFO "echo_open(): Returning\n");

     // must return number of bytes read (returned to the user) in the final version
	return 0;
}

static int echo_release(struct inode *inode, struct file *filp)
{
	// for debug:
	printk(KERN_INFO "echo_release(): Returning\n");
    return -1;
}

ssize_t echo_read(struct file *filep, char __user *buff, size_t count, loff_t *offp)
{
	// TODOs (Exercise 6): 
	// The read() should take the current number of echoed characters and send it to buff (in user space).
	// After that, it should also reset the counting of echoed characters.
	// Remember to return the total number of transferred bytes.

	int buf_size = 0;

	if (total_echoed = 0)
	{
		printk(KERN_INFO "echo_read(): No data to read\n");
		return total_echoed;
	}

	buf_size = strlen(_ssr_total_echoed);

	// for debug:
	printk(KERN_INFO "echo_read(): Returning\n");
    return -1;
}

ssize_t echo_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp)
{
	// TODOs (Exercise 6):
	// The write() should take the a string content stored on buff (from the user memory space) and store it in a local buffer (in the kernel memory space).
	// The transferred content can be shown in a printk function for debugging.
	// Remember to add a zero (null) character at the end of the string to signal its termination.
	// It should also count the number of stored characters and update the total number of echoed characters, for latter use by the read() function.
	// Finally, the total number of written bytes must be returned.

	// REMEMBER TO NEVER RETURN A CONSTANT 0 VALUE, OR THE CALLER COULD RETRY THE WRITE OPERATION ENDLESSLY.

	// INternal kernel buffer for the string to be echoed
	char write_buffer[WR_BUF_SIZE];
	int num_characters;

	// protection for limited buffer size
	// buffer larger than string to echo (ok, just echo count bytes)
	if (count < WR_BUF_SIZE)
	{
		num_characters = count-1;

		// copy 'count' bytes from user to kernel buffer (write_buffer)
		if (copy_from_user((void *)write_buffer,(const void __user *) buff,(unsigned long) count)!= 0)
		{
			return -1; // error copying
		}

		total_echoed += num_characters; // accumulate echoed characters
		sprintf(str_total_echoed, "%d\n", total_echoed); // convert to ascii string

		write_buffer[num_characters] = 0; // add null termination
		printk(KERN_INFO "Wrote %s, with size %ld. Total of %d characters echoed", write_buffer, count-1, total_echoed); // print echoed string

		return count; // return number of bytes written
	}
	else // write just the size of the buffer to each call
	{
		num_characters = WR_BUF_SIZE - 1;

		// the remainder will be writte by subsequent calls to write()
		// copy 'WR_BUF_SIZE' bytes from user to kernel buffer (write_buffer)
		if (copy_from_user((void *)write_buffer,(const void __user *) buff,(unsigned long) WR_BUF_SIZE)!=0)
		{
			return -1; // error copying
		}

		total_echoed += num_characters; // accumulate echoed characters
		sprintf(str_total_echoed, "%d\n", total_echoed); // convert to ascii string

		write_buffer[num_characters] = 0; // add null termination
		printk(KERN_INFO "Wrote %s, with size %ld. Total of %d characters echoed", write_buffer, WR_BUF_SIZE-1, total_echoed); // print echoed string

		return WR_BUF_SIZE; // return number of bytes written

	}
}


/** module functions  **/
static int echo_init(void)
{
	int return_of_alloc_chrdev_region = -1;
	int return_of_cdev_add = -1;

	// TODOs (Exercise 2): Register the device driver so that:
	// - Driver name is echo 
	// - Major number is dynamically assigned
	// - Minor number starts from 0
	// - Only one device needs to be managed by the driver

	// if (return_of_alloc_chrdev_region < 0) {
	// 	printk(KERN_ERR "Failed to register echo device driver\n");
	// 	return return_of_alloc_chrdev_region;
	// }

    // TODOs (Exercise 4): Create struture to represent char devices

    // TODO (Exercise 4): Register character device into the kernel
	
	// if (return_of_cdev_add < 0){
	// 	printk(KERN_ERR "Failed to register character device\n");
	// 	return return_of_cdev_add;
	// }

	// TODO (Exercise 2) print "Echo device driver registered with major number X" to the kernel logging buffer so that:
	// - X is the obtained major number during registration
	// - Message printed using the informational log evel
	
	printk(KERN_INFO "Loading echo module\n");

	return 0;
}

static void echo_exit(void)
{
    // TODO (Exercise 4): Deregister character device
	
    // TODO (Exercise 2): Deregister the device driver's device numbers

	// TODO (Exercise 2): Print "Echo device driver deregistered" to the 
	// kernel logging buffer using the information log level

	printk(KERN_INFO "Unloading echo module\n");

}

module_init(echo_init);
module_exit(echo_exit);
