#ifndef WIN32

#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>


void linux_set_baudrate(int fd, int baud)
{
	struct termios2 tio;
	ioctl(fd, TCGETS2, &tio);
	tio.c_cflag &= ~CBAUD;
	tio.c_cflag |= BOTHER;
	tio.c_ispeed = baud;
	tio.c_ospeed = baud;
	ioctl(fd, TCSETS2, &tio);
}
#endif

