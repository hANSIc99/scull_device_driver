#ifndef __scull_h
#define __scull_h

#include <linux/init.h>
#include <linux/module.h> /* struct file operations */
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/fs.h> /* registering the device number, contains inode */
#include <linux/types.h> /*  dev_t type */
#include <linux/cdev.h>  /* cdev structure */
#include <linux/slab.h> /* kmalloc */
#include <linux/mutex.h> /* init mutex */
#include <linux/kernel.h> /* container_of macro */
#include <asm/uaccess.h>	/* copy_to_user */

#define SCULL_MINOR 0
#define SCULL_NR_DEVICES 4
#define SCULL_QUANTUM 4000
#define SCULL_QSET 1000

/* each structure of this type represents a device */
struct scull_dev {
	struct scull_qset *data;	/*  pointer to first quantum set */
	int quantum;			/*  the current quantum size */
	int qset;			/*  the current array size */
	unsigned long size;		/*  amount of data stored here */
	unsigned int access_key;	/*  used by scullid and scullpriv */
	struct mutex mutex;		/*  mutual exclusion semaphore */
	struct semaphore sem;		/*  mutual exclusion semaphore */
	struct cdev cdev;		/*  Char device structure */
};

/*  The cdev strcuture represents a char device
 *
 *
 *
 * */


/* the quantum set represents a list
 * 
 * each quantum stores allocates 4000 bytes and the
 * list contains 1000 quantums.
 *
 * on a 64-bit-machine, a single byte consumes 12.000 bytes of memory
 * 4000 for the quantum and 8000 (8byte * 1000) for the quantum set 
 *
 * scull_device->data
 * ........scull_qset->data (contains 1000 pointer)
 * ................quantum (size = 4000 byte)
 *       
 */

struct scull_qset {
	void **data;
	struct scull_qset *next;
};


int scull_minor = SCULL_MINOR;
int scull_nr_devices = SCULL_NR_DEVICES;
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET;
int scull_p_init(dev_t dev);
int scull_access_init(dev_t dev);
int scull_open(struct inode *inode, struct file *filp);
int scull_release(struct inode *inode, struct file *filp);
struct scull_qset *scull_follow(struct scull_dev *dev, int n);
ssize_t scull_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);

/* sudo insmod scull.ko howmany=12 drv_name="Driver1" */
/* sudo rmmod scull */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stephan");
MODULE_DESCRIPTION("Modul-Beschreibung hier einsetzen");


#endif
