#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/types.h>

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


int main()
{
	int fd, retr;
	ssize_t rt;
	char *buf = (char*) calloc (100, sizeof(char));

	//Opens the file
	fd = open("/dev/imu_char", O_RD);
	if (fd<0)
	{
		printf("Device not supported by the Kernel\n");
		return 0;
	}
	// Read operation is provoked
	rt = read(fd, buf, 3);

	//IOCTL operation is provoked
	ioctl(fd, MAG_1, (int32_t*) &retr);
	printf("The value is %d", retr);

	ioctl(fd, MAG_2, (int32_t*) &retr);
        printf("The value is %d", retr);

	ioctl(fd, MAG_3, (int32_t*) &retr);
        printf("The value is %d", retr);

	//closes the device
        close(fd);

}
