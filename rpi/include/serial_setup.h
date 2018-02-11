#ifndef SERIAL_SETUP_H
#define SERIAL_SETUP_H

#include <cstring>
#include <cstdlib>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <fcntl.h>
#include <cstdio>
#include <time.h>
#include <unistd.h>
#include <errno.h>

/** error codes */
#define ERRLOPEN     -1
#define ERRLLOCK     -2
#define ERRPOPEN     -3
#define ERRPFCNTL    -4
#define ERRPIOCTL    -5
#define ERRPFLUSH    -6
#define ERRPSETATTR  -7
#define ERRPCLOSE    -8
#define ERRLUNLOCK   -16
#define ERRLREMOVE   -32

/** Serial port lock file name */
#define LOCKNAME "/tmp/serial_lock"
/** Baudrate for serial connection */
#define TTYBAUDRATE B38400

/** Open serial port and create lockfile */
int open_port(const char*);
/** Initialize port (linux handling) */
int setup_port(int);
/** Closes serial port and removes lock file */
int close_port(int);

#endif
