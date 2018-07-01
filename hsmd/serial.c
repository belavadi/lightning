/*
 * This program sends the string from command line argument argv[1] to the serial port
 * If there are no arguments - sends "Nothing"
 * Can be used to debug communication between hardware device and our hsmd daemon
 * Add serial_read function
 */

#include "serial.h"

// Arduimo is hardwired to /dev/ttyACM0
static char serialport1[100] = {[0 ... 99] = 0};
static char nullchar[100] = {[0 ... 99] = 0};

char device_paths[][25] = {
		"/dev/cu.usbmodem14311",
		"/dev/cu.usbmodem1442",
		"/dev/ttyACM0",
        "/dev/ttyACM1"
	};

void serial_write(char * msg){
    int fd;/*File Descriptor*/

    //Scan for ports, make sure the device is there:

    printf("Try to compare\n");
    if (0 == strcmp(serialport1,nullchar))
    {
        for (int i=0; i<4; i++)
        {
            fd = open(device_paths[i], O_RDWR | O_NOCTTY | O_NDELAY);
                                    /* O_RDWR Read/Write access to serial port           */
                                    /* O_NOCTTY - No terminal will control the process   */
                                    /* O_NDELAY -Non Blocking Mode,Does not care about-  */
                                    /* -the status of DCD line,Open() returns immediatly */

            if(fd == -1)                        /* Error Checking */
            {
                printf("Scan: Device not found at%s",device_paths[i]);
            }
            else
            {
                strcpy(serialport1, device_paths[i]);
                printf("\nDevice found at%s\n",serialport1);
                break;
            }
            if (i==3)
            {
                printf("Scan: Device not connected, or unrelated serial error.\n");
                close(fd);
                return;
            }
        }
    }
    else
    {
        fd = open(serialport1, O_RDWR | O_NOCTTY | O_NDELAY);
        if(fd == -1)
        {
            printf("Skipped Scan, Error while opening serial device file. %s\n", serialport1);
            close(fd);
            return;
        }
    }

    /*---------- Setting the Attributes of the serial port using termios structure --------- */

    struct termios SerialPortSettings;  /* Create the structure                          */

    tcgetattr(fd, &SerialPortSettings); /* Get the current attributes of the Serial port */

    cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
    cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

    SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;    /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

    SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

    /*------------------------------- Write data to serial port -----------------------------*/

    int  bytes_written  = 0;    /* Value for storing the number of bytes written to the port */

    bytes_written = write(fd,msg,strlen(msg));/* use write() to send data to port                                            */
                                     /* "fd"                   - file descriptor pointing to the opened serial port */
                                     /* "write_buffer"         - address of the buffer containing data              */
                                     /* "sizeof(write_buffer)" - No of bytes to write                               */
    close(fd);/* Close the Serial port */
}

int serial_read(char *result, size_t length){
    int fd;/*File Descriptor*/
    printf("serial_read %s\n", result);

    //scan for ports make sure the device is there

    if (0 == strcmp(serialport1,nullchar))
    {
        for (int i=0; i<sizeof(device_paths)/sizeof(device_paths[0]); i++)
        {
            fd = open(device_paths[i], O_RDWR | O_NOCTTY | O_NDELAY);
                                    /* O_RDWR Read/Write access to serial port           */
                                    /* O_NOCTTY - No terminal will control the process   */
                                    /* O_NDELAY -Non Blocking Mode,Does not care about-  */
                                    /* -the status of DCD line,Open() returns immediatly */

            if(fd == -1)                        /* Error Checking */
            {
                printf("Device not found at%s",device_paths[i]);
            }
            else
            {
                strcpy(serialport1, device_paths[i]);
                printf("\nDevice found at%s\n",serialport1);
                break;
            }
            if (i==3)
            {
                printf("Device not connected, or unrelated serial error.\n");
                close(fd);
                return 1;
            }
        }
    }
    else
    {
        fd = open(serialport1, O_RDWR | O_NOCTTY | O_NDELAY);
        if(fd == -1)
        {
            printf("Error while opening serial device file. %s\n", serialport1);
            close(fd);
            return 1;
        }
    }

    /*---------- Setting the Attributes of the serial port using termios structure --------- */

    struct termios SerialPortSettings;  /* Create the structure                          */

    tcgetattr(fd, &SerialPortSettings); /* Get the current attributes of the Serial port */

    cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
    cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

    SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;    /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

    SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

    /*------------------------------- Read data to serial port -----------------------------*/

    //char buf[250]; /*Store the buffer in here */
    int waited_sec = 0;
    int max_wait_sec = 10;
    int bytes_read = 0;
    int offset = 0;

	while (waited_sec < max_wait_sec && offset < length - 1) {
		bytes_read = read(fd, result+offset, 1);
		if (bytes_read > 0) { // how can buf[offset] be -32 here??
			//printf("FIXMEH: read %d chars, offset is now %d: %s (last char %d)\n", bytes_read, offset, result, result[offset]);
			if (result[offset] == '\n') {
				break; // end of message
			}
			offset++;
			continue;
		}
		sleep(1);
		waited_sec++;
    }

    close(fd);/* Close the Serial port */
    if (bytes_read > 0)
    {
        printf("Read %d: \"%s\"\n", bytes_read, result);
        return 0;
    }
    return 1;
}

