#include "scull.h" 

extern int scull_minor;
extern int scull_nr_devices;
extern int scull_quantum;
extern int scull_qset;

static char* drv_name = "scull";
static int howmany = 1;
static dev_t device_number = 0;
module_param(howmany, int, S_IRUGO);
module_param(drv_name, charp, S_IRUGO);

struct scull_dev *scull_devices;

static void scull_setup_cdev(struct scull_dev *dev, int index);
static void __exit scull_exit(void);
static int __init scull_init(void);

struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.open = scull_open,
	.release = scull_release,
	/* .read = 
	.write = */
};

/* FILE OPERATION METHODS */
/* open the char device driver (file) */
int scull_open(struct inode *inode, struct file *filp){

	struct scull_dev *dev;	/* device information */

	/* inode->i_cdev points to the previsously created cdev struct in setup cdev */
	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev;	/* for other methods */

	/* now trim the lenght to 0 is open was write only */
	/*  if one of the fields is = 1 */

	

	if((filp->f_flags & O_ACCMODE) == O_WRONLY){
#if 0
		if(down_interruptible(&dev->mutex))
			return -ERESTARTSYS;
	/* 	scull_trim(dev); */
		up(&dev->mutex);
#endif
	}
	return 0;	/* sucess */
}

int scull_release(struct inode *inode, struct file *filp){
	return 0;
}

ssize_t scull_read(struct file *filp, char __user *buff, size_t count, loff_t *offp){

	/* this method reads data from the char-driver */
	/* offp = actual file position the user is accesing  */
	/* count = size of the requested data */

	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;	/* the first list item */
	int quantum = dev->quantum;	/* the current quantum size (max 4000) */
        int qset = dev->qset;		/* the current array size (max 1000)  */
	int itemsize = quantum * qset;	/* actual size = 4000 * [1 < x < 1000] =< 4.000.000 */
	int item, s_pos, q_pos, rest;
	ssize_t retval;

	/* if the current read position is greater then the device size */
	if(*offp >= dev->size)
		goto out;
	/* if the position pointer is outside  */
	if(*offp + count > dev->size)
		count = dev->size - *offp;

	/* find listitem, qset index, and offset in the quantum */
	item = (long)*offp / itemsize;	/* deflt:  file_pos /  4*10⁶ = number of listitem */
	rest = (long)*offp % itemsize;	/* number of bytes for the last listitem */
	s_pos = rest / quantum;		/* actual pointer to current quantum (0 < ptr < 999) */
	q_pos = rest % quantum;		/* quantum is filled up to this position (0 < q_pos < 3999) */ 
	dptr = scull_follow(dev, item);

	if (dptr == NULL || !dptr->data || !dptr->data[s_pos])
		goto out;	/* dont fill holes */


	/* read only up to the end of this quantum */
	/* the method read only one quantum at a time */
	if(count > quantum - q_pos)
		count = quantum - q_pos;


	/* the function return the number ob bytes that have been copied,
	 * on success case its 0
	 *
	 * first argument =  target 
	 * second argument =  source
	 * third argument = number of bytes requested */
	
	
	if(copy_to_user(buff, dptr->data[s_pos] + q_pos, count)) {
		retval = -EFAULT;
		goto out;
	}
	
	/*  offp must represent the current file position
	 *  in success case therefore: offp = count */

	*offp += count;	/* update file position pointer */
	retval = count;	/* return readed bytes */

	out:
	return retval;
	/*  up(&dev->mutex); */
	

}


ssize_t scull_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp){

	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum;	/* the current quantum size (max 4000) */
	int qset = dev->qset;		/* the current array size (max 1000)  */
	int itemsize = quantum * qset;	/* actual size = 4000 * [0 < x < 999] =< 4.000.000 */
	int item, s_pos, q_pos, rest;
	ssize_t retval = -ENOMEM;	/* value used in "goto out" statements */

	/* find listitem, qset index and offset in the quantum */
	
	item = (long)*offp / itemsize; /* deflt:  file_pos /  4*10⁶ = number of listitem */
	rest = (long)*offp % itemsize; /* number of bytes for the last listitem */	
	s_pos = rest / quantum; /* actual pointer to current quantum (0 < ptr < 999) */
	q_pos = rest % quantum; /* quantum is filled up to this position (0 < q_pos < 3999) */ 

	/* follow the list up to the right position */

	dptr = scull_follow(dev, item);

	if(dptr == NULL)
		goto out;
	if(!dptr->data){
		/* allocates one quantum: 1000 bytes */
		dptr->data = kmalloc(qset * sizeof(char*), GFP_KERNEL);
		if (!dptr->data) /* check if kmalloc suceeded */
			goto out;
		memset(dptr->data, 0, qset * sizeof(char*));
	}

	/* find the pointer ptr:  0 < ptr < 999 */

	if(!dptr->data[s_pos]){ /* if the actual pointer is 0 */
		/* allocate a quantum: 4000 bytes */
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
		if(!dptr->data[s_pos])
			goto out;
	}

	/* write only up to the end of thos quantum */

	if(count > quantum - q_pos)
		count = quantum - q_pos;

	if(copy_from_user(dptr->data[s_pos]+q_pos, buff, count)){
		retval = -EFAULT;
		goto out;
	}

	*offp += count;
	retval = count;

	/*  update the size */
	if(dev->size < *offp)
		dev->size = *offp;


out:
	return retval;
}




struct scull_qset *scull_follow(struct scull_dev *dev, int n){

struct scull_qset *qs = dev->data;

	if(!qs){
	/* allocate the first qset explicitly if needed */
		qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
			if(qs == NULL)
				return NULL;	/* allocation failed */
		memset(qs, 0, sizeof(struct scull_qset));
	}

/* Follow the list */

	while (n--){
		if(!qs->next){	/* last element found */
			qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
			if(qs->next == NULL)	/* allocation failed */
				return NULL;
			memset(qs->next, 0, sizeof(struct scull_qset));
		}
		qs = qs->next;
	/*  necessary ? */
	continue;
	}

	return qs;

}

static int __init scull_init(void){

	int result, i;
	dev_t dev = 0;

	result = alloc_chrdev_region(&device_number , scull_minor,scull_nr_devices, drv_name);	

	if(result < 0) {
		printk(KERN_ALERT "scull: can't get major %d\n", MAJOR(device_number));
		return result;
	}



	/* register char-devices */
	scull_devices = kmalloc(scull_nr_devices * sizeof(struct scull_dev), GFP_KERNEL);
	/* error checking */
	if(!scull_devices){
		result = -ENOMEM;
		goto fail;
	}

	/* set memory to zero */
	memset(scull_devices, 0, scull_nr_devices * sizeof(struct scull_dev));

	/* initializsation */
	for ( i=0; i< scull_nr_devices; i++){

		scull_devices[i].quantum = scull_quantum;
		scull_devices[i].qset = scull_qset;
		mutex_init(&scull_devices[i].mutex);	/* keine Ahnung was ein Mutex ist */
		scull_setup_cdev(&scull_devices[i], i);	
	}

	dev = MKDEV(MAJOR(device_number), MINOR(device_number) + scull_nr_devices); 

	/*  Baustelle */

	/* dev += scull_p_init(dev);
	dev += scull_access_init(dev); */
	
		


	printk(KERN_ALERT "Hello World by Stephan!\n");
	printk(KERN_ALERT "string driver-name = %s, howmany=%d\n", drv_name, howmany);
	printk(KERN_ALERT "aktueller prozess: %s", current->comm);
	printk(KERN_ALERT "major numder: %d, minor number: %d",
	     MAJOR(device_number), MINOR(device_number) );
#if 0
	printk(KERN_ALERT "The current process is \"%s\" (pid %i)\n",
			current->comm, current->pid);
#endif 
	return 0;

fail:
	scull_exit();
	return result;
}

/* setup of the char-device structure */
static void scull_setup_cdev(struct scull_dev *dev, int index){

	int err, devno = MKDEV(MAJOR(device_number), MINOR(device_number) + index);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE ;
	dev->cdev.ops = &scull_fops;
	/* final step to tell the kernel that a char device is created */
	err = cdev_add (&dev->cdev, devno, 1);

	if(err)
		printk(KERN_ALERT "scull: Error %d adding scull %d", err, index);

}

/* scull trim is invoked by scull_open and the cleanup-function to free the whole data */
int scull_trim(struct scull_dev *dev){

	struct scull_qset *next, *dptr;
	int qset = dev->qset;	/* dev is not-null */
	int i;

	for (dptr = dev->data; dptr; dptr = next){
		if(dptr->data){
			for (i = 0; i<qset; i++)
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data =NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;

	return 0;
	

}

static void __exit scull_exit(void){


	int i;
	printk(KERN_ALERT "scull: scull_exit() calles");

	if(scull_devices) {
		for (i = 0; i < scull_nr_devices; i++){
			scull_trim(scull_devices+1);
			cdev_del(&scull_devices[i].cdev);
		}
		kfree(scull_devices);
	}

	unregister_chrdev_region(device_number, scull_nr_devices);

	printk(KERN_ALERT "Goodbye by Stephan\n");

}


module_init(scull_init);
module_exit(scull_exit);
