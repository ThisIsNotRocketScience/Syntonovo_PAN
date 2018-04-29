#ifndef WIN32

#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <termios.h>
#include <iostream>
#include <list>

using namespace std;

static string get_driver(const string& tty) {
	struct stat st;
	string devicedir = tty;

	// Append '/device' to the tty-path
	devicedir += "/device";

	// Stat the devicedir and handle it if it is a symlink
	if (lstat(devicedir.c_str(), &st) == 0 && S_ISLNK(st.st_mode)) {
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		// Append '/driver' and return basename of the target
		devicedir += "/driver";

		if (readlink(devicedir.c_str(), buffer, sizeof(buffer)) > 0)
			return basename(buffer);
	}
	return "";
}

static void register_comport(list<string>& comList, list<string>& comList8250, const string& dir) {
	// Get the driver the device is using
	string driver = get_driver(dir);

	// Skip devices without a driver
	if (driver.size() > 0) {
		string devfile = string("/dev/") + basename(dir.c_str());

		// Put serial8250-devices in a seperate list
		if (driver == "serial8250") {
			comList8250.push_back(devfile);
		}
		else
			comList.push_back(devfile);
	}
}

static void probe_serial8250_comports(list<string>& comList, list<string> comList8250) {
	struct serial_struct serinfo;
	list<string>::iterator it = comList8250.begin();

	// Iterate over all serial8250-devices
	while (it != comList8250.end()) {

		// Try to open the device
		int fd = open((*it).c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY);

		if (fd >= 0) {
			// Get serial_info
			if (ioctl(fd, TIOCGSERIAL, &serinfo) == 0) {
				// If device type is no PORT_UNKNOWN we accept the port
				if (serinfo.type != PORT_UNKNOWN)
					comList.push_back(*it);
			}
			close(fd);
		}
		it++;
	}
}

list<string> getComList() {
	int n;
	struct dirent **namelist;
	list<string> comList;
	list<string> comList8250;
	const char* sysdir = "/sys/class/tty/";

	// Scan through /sys/class/tty - it contains all tty-devices in the system
	n = scandir(sysdir, &namelist, NULL, NULL);
	if (n < 0)
		perror("scandir");
	else {
		while (n--) {
			if (strcmp(namelist[n]->d_name, "..") && strcmp(namelist[n]->d_name, ".")) {

				// Construct full absolute file path
				string devicedir = sysdir;
				devicedir += namelist[n]->d_name;

				// Register the device
				register_comport(comList, comList8250, devicedir);
			}
			free(namelist[n]);
		}
		free(namelist);
	}

	// Only non-serial8250 has been added to comList without any further testing
	// serial8250-devices must be probe to check for validity
	probe_serial8250_comports(comList, comList8250);

	// Return the lsit of detected comports
	return comList;
}
int USB = 0;
extern void ByteReceived(unsigned char byte);
void UpdateComm()
{
	if (USB == 0) return;

	int bytes_avail;
	ioctl(USB, FIONREAD, &bytes_avail);
	unsigned char bytes[30000];
	while (bytes_avail > 0)
	{
		int toread = bytes_avail;
		if (toread > 30000) toread = 30000;

		int n = read(USB, &bytes, toread);
		for (int i = 0; i < toread; i++)
		{
			ByteReceived(bytes[i]);
		}
		bytes_avail -= toread;
	}

};

void OpenIt(string port)
{
	USB = open(port.c_str(), O_RDWR | O_NOCTTY);

	struct termios tty;
	struct termios tty_old;
	memset(&tty, 0, sizeof tty);

	/* Error Handling */
	if (tcgetattr(USB, &tty) != 0) {
		std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
	}

	/* Save old tty parameters */
	tty_old = tty;

	tty.c_ispeed = tty.c_ospeed = 1000000;

	/* Setting other Port Stuff */
	tty.c_cflag &= ~PARENB;            // Make 8n1
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

	tty.c_cflag &= ~CRTSCTS;           // no flow control
	tty.c_cc[VMIN] = 1;                  // read doesn't block
	tty.c_cc[VTIME] = 5;                  // 0.5 seconds read timeout
	tty.c_cflag |= CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

	/* Make raw */
	cfmakeraw(&tty);

	tty.c_cflag |= (CLOCAL | CREAD);   // Enable the receiver and set local mode
	tty.c_cflag &= ~CSTOPB;            // 1 stop bit
	tty.c_cflag &= ~CRTSCTS;           // Disable hardware flow control
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 2;



	/* Flush Port, then applies attributes */
	tcflush(USB, TCIFLUSH);
	if (tcsetattr(USB, TCSANOW, &tty) != 0) {
		std::cout << "Error " << errno << " from tcsetattr" << std::endl;
	}



}
void OpenComm()
{
	printf("Scanning for serialports..\n");
	list<string> l = getComList();

	string notthatone = "/dev/ttyAMA0";
	list<string>::iterator it = l.begin();
	while (it != l.end())
	{
		cout << *it << endl;
		if (*it != notthatone)
		{
			printf("trying to open %s\n", (*it).c_str());
			OpenIt(*it);
			return;
		}
		else
		{
			printf("skipping %s!\n", notthatone.c_str());
		}
		it++;
	}

}

#endif
