#include <linux/module.h>
#include <linux/fs.h>		
#include <linux/errno.h>	
#include <linux/types.h>	
#include <linux/fcntl.h>	
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/string.h>
#include <linux/types.h>


#define MEMSIZE 1024 // Size of Ram disk
# define KERNSIZE (512)
int rb_major=0; //Variable for Major Number
int  sectsize = 512; 


/*Doing partitioning*/

#define SECTOR_SIZE 512
#define MBR_SIZE SECTOR_SIZE
#define MBR_DISK_SIGNATURE_OFFSET 440
#define MBR_DISK_SIGNATURE_SIZE 4
#define PARTITION_TABLE_OFFSET 446
#define PARTITION_ENTRY_SIZE 16 
#define PARTITION_TABLE_SIZE 64 
#define MBR_SIGNATURE_OFFSET 510
#define MBR_SIGNATURE_SIZE 2
#define MBR_SIGNATURE 0xAA55

typedef struct
{
	unsigned char boot_type; // 0x00 - Inactive; 0x80 - Active (Bootable)
	unsigned char start_head;
	unsigned char start_sec:6;
	unsigned char start_cyl_hi:2;
	unsigned char start_cyl;
	unsigned char part_type;
	unsigned char end_head;
	unsigned char end_sec:6;
	unsigned char end_cyl_hi:2;
	unsigned char end_cyl;
	unsigned int abs_start_sec;
	unsigned int sec_in_part;
} PartEntry;

typedef PartEntry PartTable[4];

static PartTable def_part_table =
{
	{
		boot_type: 0x00,
		start_head: 0x00,
		start_sec: 0x2,
		start_cyl: 0x00,
		part_type: 0x83,
		end_head: 0x00,
		end_sec: 0x20,
		end_cyl: 0x09,
		abs_start_sec: 0x00000001,
		sec_in_part: 0x0000013F
	},
	{
		boot_type: 0x00,
		start_head: 0x00,
		start_sec: 0x1,
		start_cyl: 0x14,
		part_type: 0x83,
		end_head: 0x00,
		end_sec: 0x20,
		end_cyl: 0x1F,
		abs_start_sec: 0x00000280,
		sec_in_part: 0x00000180
	},
	{
	},
	{
	}
};


static void copy_mbr(u8 *disk)
{
	memset(disk, 0x0, MBR_SIZE);
	*(unsigned long *)(disk + MBR_DISK_SIGNATURE_OFFSET) = 0x36E5756D;
	memcpy(disk + PARTITION_TABLE_OFFSET, &def_part_table, PARTITION_TABLE_SIZE);
	*(unsigned short *)(disk + MBR_SIGNATURE_OFFSET) = MBR_SIGNATURE;
}

/* Structure associated with Block ramdevice*/
struct rb_dev 
{
	int size;
	u8 *data;
	spinlock_t lock;
	struct request_queue *queue;
	struct gendisk *gd;

}ramdevice;

struct rb_dev *x;

static int my_open(struct block_ramdevice *x, fmode_t mode)	 
{
	int ret=0;
	printk(KERN_INFO "rb : open \n");
	goto out;

	out :
	return ret;

}

static void my_release(struct gendisk *disk, fmode_t mode)
{
	
	printk(KERN_INFO "rb : closed \n");

}

static struct block_ramdevice_operations fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
};


int ramramdevice_init(void)
{
	(ramdevice.data) = vmalloc(MEMSIZE*sectsize);

	/* Setup its partition table */
	copy_mbr(ramdevice.data);

	goto out;

	out:
	return MEMSIZE;	
}

static int rb_transfer(struct request *req)
{
	int dir = rq_data_dir(req);
	int ret = 0;
	/*starting sector
	 *where to do operation*/
	
	sector_t start_sector = blk_rq_pos(req);

	unsigned int sector_cnt = blk_rq_sectors(req); /* no of sector 
							*on which opn to be done*/

	struct bio_vec bv;
	#define BV_PAGE(bv) ((bv).bv_page)
	#define BV_OFFSET(bv) ((bv).bv_offset)
	#define BV_LEN(bv) ((bv).bv_len)
	struct req_iterator iter;
	sector_t sector_offset;
	unsigned int sectors;
	u8 *buffer;
		
	sector_offset = 0;
	rq_for_each_segment(bv, req, iter)
	{
		buffer = page_address(BV_PAGE(bv)) + BV_OFFSET(bv);
		if (BV_LEN(bv) % (sectsize) != 0)
		{
			printk(KERN_ERR"Bio size is not a multiple of sector size\n");
			ret = -EIO;
		}
		sectors = BV_LEN(bv) / sectsize;
		printk(KERN_DEBUG "ram ramdevice: Start Sector: %llu, Sector Offset: %llu;\
		Buffer: %p; Length: %u sectors\n",\
		(unsigned long long)(start_sector), (unsigned long long) \
		(sector_offset), buffer, sectors);
		
		if (dir == WRITE) /* Write to the ramdevice */
		{
			
			memcpy((ramdevice.data)+((start_sector+sector_offset)*sectsize)\
			,buffer,sectors*sectsize);		
			
		}
		else /* Read from the ramdevice */
		{
			
			memcpy(buffer,(ramdevice.data)+((start_sector+sector_offset)\
			*sectsize),sectors*sectsize);	
		}
		sector_offset += sectors;
	}
	
	if (sector_offset != sector_cnt)
	{
		printk(KERN_ERR "ramramdevice: Bio info doesn't match with the request info");
		ret = -EIO;
	}

return ret;
}


/** request handling function**/
static void dev_request(struct request_queue *q)
{
	struct request *req;
	int error;


	while ((req = blk_get_request(q)) != 0) /*check active request 
						      *for data transfer*/
	{
	

		error=rb_transfer(req);// transfer the request for operation
		__blk_end_request_all(req, error); // end the request
	}

}



void ramdevice_setup(void)
{
	ramramdevice_init();
	rb_major = register_blkdev(rb_major, "dof");// major no. allocation
	printk(KERN_ALERT "Major Number is : %d",rb_major);
	spin_lock_init(&ramdevice.lock); // lock for queue
	ramdevice.queue = blk_put_queue( dev_request, &ramdevice.lock); 

	ramdevice.gd = alloc_disk(8); // gendisk allocation
	
	(ramdevice.gd)->major=rb_major; // major no to gendisk
	ramdevice.gd->first_minor=0; // first minor of gendisk
	ramdevice.gd->minors=2;
	ramdevice.gd->fops = &fops;
	ramdevice.gd->private_data = &ramdevice;
	ramdevice.gd->queue = ramdevice.queue;
	ramdevice.size= ramramdevice_init();
	printk(KERN_INFO"ramdevice SIZE IS %d",ramdevice.size);	
	sprintf(((ramdevice.gd)->disk_name), "dof");
	set_capacity(ramdevice.gd, MEMSIZE*(KERNSIZE/sectsize));  
	add_disk(ramdevice.gd);

}

static int __init rb_init(void)
{	
	int ret=0;
	ramdevice_setup();
	
	goto out;
	
	out:	
	return ret;

}

void ramramdevice_cleanup(void)
{
	vfree(ramdevice.data);
}


void __exit rb_exit(void)
{	
	del_gendisk(ramdevice.gd);
	unregister_blkdev(rb_major, "dof");
	put_disk(ramdevice.gd);	
	blk_cleanup_queue(ramdevice.queue);
	ramramdevice_cleanup();
	spin_unlock(&ramdevice.lock);	
				
}


module_init(rb_init);
module_exit(rb_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("VAIBHAV <h20201400238@hyderabad.bits-pilani.ac.in");
MODULE_DESCRIPTION("RAM BLOCK DRIVER");
