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
#include <linux/proc_fs.h>	/* debugging with read only files in /proc */
#include <linux/seq_file.h>	/* struct seq_file */
#include <linux/module.h>

#define SCULL_MINOR 0
#define SCULL_NR_DEVICES 4
#define SCULL_QUANTUM 4000
#define SCULL_QSET 1000

#undef PDEBUG
#ifdef SCULL_DEBUG
#	ifdef __KERNEL__
	/* This one if debugging is on, and kernel space */
#	define PDEBUG(fmt, args...) printk( KERN_DEBUG "scull: " fmt, ## args)
#  else
	/* This one is for user space */
#	define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...)	/*  not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...)	/* nothing: it's a placeholder */


MODULE_AUTHOR("Alessandro Rubini, Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");



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
int scull_read_procmem(char* buf, char **start, off_t offset, int count, int *eof, void *data);
struct scull_qset *scull_follow(struct scull_dev *dev, int n);
ssize_t scull_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);
ssize_t scull_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);

/* proc/seq_file operations */
int scull_proc_open(struct inode *inode, struct file *file);
void scull_seq_stop(struct seq_file *s, void *v);
int scull_seq_show(struct seq_file *s, void *v);
void *scull_seq_next(struct seq_file *sfile, void *v, loff_t *pos);
void *scull_seq_start(struct seq_file *s, loff_t *pos);
void scull_create_proc_entry(void);

/* sudo insmod scull.ko howmany=12 drv_name="Driver1" */
/* sudo rmmod scull */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stephan");
MODULE_DESCRIPTION("Modul-Beschreibung hier einsetzen");


#endif
