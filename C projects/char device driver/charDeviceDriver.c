
#include <linux/init.h>   // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/device.h> // Header to support the kernel Driver Model
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include <linux/fs.h>	 // Header for the Linux file system support
#include <asm/uaccess.h>  // Required for the copy to user function
#include <linux/mutex.h>  // Required for the mutex functionality
#include <linux/slab.h>
#include "ioctl.h"
#include "charDeviceDriver.h"

MODULE_LICENSE("GPL");										  ///< The license type -- this affects available functionality

struct QNode
{
	char *key;
	struct QNode *next;
	int length;
};
struct Queue
{
	struct QNode *front, *rear;
	int size;
};
struct QNode *newNode(char *k, int length)
{
	struct QNode *temp = kmalloc(sizeof(struct QNode), GFP_KERNEL);
	size_t i;
	if (temp == NULL)
	{
		printk(KERN_INFO "charDeviceDriver - newNode: kmalloc returned NULL\n");
		return NULL;
	}
	temp->key = kmalloc(length, GFP_KERNEL);
	if(temp->key == NULL){
		printk(KERN_INFO "charDeviceDriver - newNode->key: kmalloc returned NULL\n");
		return NULL;
	}
	for ( i = 0; i < length; i++)
	{
		*(temp->key + i)= *(k + i);
	}
	temp->length = length;
	temp->next = NULL;
	return temp;
}

// A utility function to create an empty queue
struct Queue *createQueue(void)
{
	struct Queue *q;
	q = kmalloc(sizeof(struct Queue), GFP_KERNEL);
	if (q == NULL)
	{
		printk(KERN_INFO "charDeviceDriver - createQueue: kmalloc returned NULL\n");
		return NULL;
	}
	q->front = q->rear = NULL;
	q->size = 0;

	return q;
}

// The function to add a key k to q
void enQueue(struct Queue *q, char *k, int length)
{
	struct QNode *temp;
	temp = newNode(k, length);
	if (temp == NULL)
	{
		printk(KERN_INFO "charDeviceDriver - enQueue: newNode returned NULL");
		return;
	}
	// If queue is empty, then new node is front and rear both
	if (q->rear == NULL)
	{
		q->front = q->rear = temp;
		q->size++;
		return;
	}

	// Add the new node at the end of queue and change rear
	q->rear->next = temp;
	q->rear = temp;
	q->size++;
}

// Function to remove a key from given queue q
void deQueue(struct Queue *q)
{
	struct QNode *temp;
	// If queue is empty, return NULL.
	if (q == NULL)
	{
		printk(KERN_INFO "charDeviceDriver - deQueue: q == NULL\n");
		return;
	}
	if (q->front == NULL)
	{
		printk(KERN_INFO "charDeviceDriver - deQueue: q->front == NULL\n");
		return;
	}
	// Store previous front and move front one node ahead
	temp = q->front;
	kfree(temp);

	q->front = q->front->next;

	// If front becomes NULL, then change rear also as NULL
	if (q->front == NULL)
		q->rear = NULL;
	q->size = q->size - 1;
	
}

struct QNode *peek(struct Queue *q)
{
	if (q != NULL && q->front != NULL)
	{
		return q->front;
	}
	else
	{
		return NULL;
	}
}

char *peekTail(struct Queue *q)
{
	if (q != NULL && q->rear != NULL)
	{
		return q->rear->key;
	}
	else
	{
		return NULL;
	}
}

static DEFINE_MUTEX(dev_mutex); ///< Macro to declare a new mutex

static long device_ioctl(struct file *file,		 /* see include/linux/fs.h */
						 unsigned int ioctl_num, /* number and param for ioctl */
						 unsigned long ioctl_param)
{
	/* 
	 * Switch according to the ioctl called 
	 */
	if (ioctl_num == RESET_COUNTER)
	{
		printk(KERN_INFO "charDeviceDriver ioctl: trying to set size to (%i)/(%lu)",total_size,ioctl_param);
		if(ioctl_param > total_size){
			LIST_MAX_SIZE = ioctl_param;
			return 0;
		}else{
			return -EINVAL;
		}
	}

	else
	{
		/* no operation defined - return failure */
		return -EINVAL;
	}
}

int init_module(void)
{
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0)
	{
		printk(KERN_ALERT "Registering char device failed with %d\n", Major);
		return Major;
	}

	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	messages = createQueue();
	if (messages == NULL)
	{
		printk(KERN_INFO "charDeviceDriver: createQueue returned NULL to init, failing...\n");
		cleanup_module();
		return 0;
	}
	printk(KERN_INFO "charDeviceDriver: createQueue returned %u to init\n", sizeof(messages));
	mutex_init(&dev_mutex); // Initialize the mutex dynamically
	return SUCCESS;
}

void cleanup_module(void)
{
	unregister_chrdev(Major, DEVICE_NAME); // unregister the major number
	// TODO free the queue
	printk(KERN_INFO "charDeviceDriver: Goodbye from the LKM!\n");
}


static int device_open(struct inode *inodep, struct file *filep)
{
	mutex_lock(&dev_mutex);
	if (numberOpens)
	{
		mutex_unlock(&dev_mutex);
		return -EBUSY;
	}
	numberOpens++;
	mutex_unlock(&dev_mutex);
	printk(KERN_INFO "charDeviceDriver: Device has been opened %d time(s)\n", numberOpens);
	try_module_get(THIS_MODULE);
	return 0;
}


static ssize_t device_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int length = 0;
    int init_length = 0;
    int bytes_read = 0;
    int result;
    struct QNode *temp;
    char *str;
    printk(KERN_INFO "charDeviceDriver: device_read of length (%i)", len);
    temp = peek(messages);
    if (temp == NULL)
    {
        printk(KERN_INFO "charDeviceDriver: read - NULL, no messages\n");
        return -EAGAIN;
    }
    printk(KERN_INFO "charDeviceDriver device_read: got message of length (%i)", temp->length);
    length = temp->length;
    init_length = length;
    if(temp->key ==NULL){
        printk(KERN_INFO "charDeviceDriver read: message was NULL\n");
        return -EAGAIN;
    }
    str = kmalloc(strlen(temp->key), GFP_KERNEL);
    strcpy(str,temp->key);
    while (length && *str) {
        /* 
         * The buffer is in the user data segment, not the kernel 
         * segment so "*" assignment won't work.  We have to use 
         * put_user which copies data from the kernel data segment to
         * the user data segment. 
         */
        result = put_user(*(str++), buffer++);
        if (result != 0) {
            return -EFAULT;
        }
        length--;
        bytes_read++;
        messages_in_list--;
    }
    deQueue(messages);
    total_size = total_size - init_length;
    printk(KERN_INFO "charDeviceDriver: bytes_read (%i)\n",bytes_read);
    return bytes_read;

}


static ssize_t device_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{    
    printk(KERN_INFO "charDeviceDriver: write: len -> (%i)\n", len);
    if (len > MSG_MAX_SIZE)
    {
        printk(KERN_INFO "charDeviceDriver: message must be smaller than %i\n", MSG_MAX_SIZE);
        return -EINVAL;
    }
    if (messages_in_list >= 1000)
    {
        printk(KERN_INFO "charDeviceDriver: This op would exced max size list");
        return -EBUSY;
    }
    enQueue(messages, (char*)buffer, len);
    total_size = total_size + len;
        messages_in_list++;

    return len;
}


static int device_release(struct inode *inodep, struct file *filep)
{
	mutex_lock(&dev_mutex);
	numberOpens--; /* We're now ready for our next caller */
	mutex_unlock(&dev_mutex); // release the mutex (i.e., lock goes up)
	module_put(THIS_MODULE);
	printk(KERN_INFO "charDeviceDriver: Device successfully closed\n");
	return 0;
}
