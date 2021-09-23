#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/random.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>

MODULE_LICENSE ("GPL");

static ssize_t ret;

// IOCTL command definitions
#define GYRO_1 _IOR('1','1',int32_t*)
#define GYRO_2 _IOR('2','2',int32_t*)
#define GYRO_3 _IOR('3','3',int32_t*)
#define ACCEL_1 _IOR('4','4',int32_t*)
#define ACCEL_2 _IOR('5','5',int32_t*)
#define ACCEL_3 _IOR('6','6',int32_t*)
#define MAG_1 _IOR('7','7',int32_t*)
#define MAG_2 _IOR('8','8',int32_t*)
#define MAG_3 _IOR('9','9',int32_t*)
#define PRESSURE _IOR('0','0',int32_t*)

// File Operations 
// This will be called when Open file is provoked
int Open (struct inode *minode, struct file *mfile)
{
	printk(KERN_ALERT "Device imu_char is Open\n");
	return 1;
}

//This function call will be provoked from user application reads the device.
ssize_t Read (struct file *mfile, char __user *buffer, size_t len, loff_t *offset)
{
	printk(KERN_ALERT "I2C Communication Initiated\n");
	get_random_bytes(&ret, len);
	return ret;
}

// IOCTL is provoked at the time of IO operations.
long IOCTL (struct file *mfile, unsigned int command, unsigned long argument)
{
	switch (command)
	{
		case GYRO_1:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret))) //sends the ret value to user application: if 0 operation fails
			{
				printk(KERN_ALERT "Error with gyro 1");
			}
			printk(KERN_ALERT "Success");
			break;
		case GYRO_2:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret)))
			{
				printk(KERN_ALERT "Error with gyro 2");
			}
			printk(KERN_ALERT "Success");
			break;
		case GYRO_3:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret)))
                        {
                                printk(KERN_ALERT "Error with gyro 3");
                        }
			printk(KERN_ALERT "Success");

	  		break;
		case ACCEL_1:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret)))
                        {
                                printk(KERN_ALERT "Error with Accel 1");
                        }
			printk(KERN_ALERT "Success");
		
			break;
		case ACCEL_2:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret)))
                        {
                                printk(KERN_ALERT "Error with Accel 2");
                        }
			printk(KERN_ALERT "Success");

			break;
		case ACCEL_3:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret)))
                        {
                                printk(KERN_ALERT "Error with Accel 3");
                        }
			printk(KERN_ALERT "Success");

			break;
		case MAG_1:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret)))
                        {
                                printk(KERN_ALERT "Error with Mag 1");
                        }
			printk(KERN_ALERT "Success");

			break;
		case MAG_2:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret)))
                        {
                                printk(KERN_ALERT "Error with Mag 2");
                        }
			printk(KERN_ALERT "Success");
			break;
		case MAG_3:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret)))
                        {
                                printk(KERN_ALERT "Error with Mag 3");
                        }
			printk(KERN_ALERT "Sucess");
			break;
		case PRESSURE:
			if (copy_to_user( (int32_t*) argument, &ret, sizeof(ret)))
                        {
                                printk(KERN_ALERT "Error with Pressure 2");
                        }
			printk(KERN_ALERT "Sucess");
			break;
		default:
			printk(KERN_ALERT "INAPPROPRIATE IO CALL");
			break;

	}
	return 0;
}

//Provoked when the close function is called
int Close (struct inode *minode, struct file *mfile)
{
	printk(KERN_ALERT "Device imu_char is closed");
	return 1;
}

//Structure definition of the file operations
struct file_operations main_file_op = {
	.owner          = THIS_MODULE,
	.open           = Open,
	.read           = Read,
        .unlocked_ioctl = IOCTL,
	.release        = Close,	

};

//Init function of the module
int main_init(void)
{
	printk(KERN_ALERT "In %s space\n", __FUNCTION__);
	register_chrdev (283, "imu_char", &main_file_op); // registers device with major number 283

	return 0;
}

// Exit function of the main module
void main_exit(void)
{
	printk (KERN_ALERT "In %s module\n", __FUNCTION__);
	unregister_chrdev(283, "imu_char"); // unregisters device
}


module_init(main_init);
module_exit(main_exit);
