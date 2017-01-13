#ifndef SERIAL_SETUP_H
#define SERIAL_SETUP_H

#include <cstring>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <fcntl.h>
#include <cstdio>
#include <time.h>
#include <unistd.h>

/** @brief Library error codes */
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

/** @brief Serial port lock file name */
#define LOCKNAME "/var/lock/serial_lock"
/** @brief Baudrate for ftdi serial connection */
#define TTYBAUDRATE B9600

/** @brief Serial port lock file descriptor */
//int lock_file;
/** @brief Open serial port and create lockfile */
int open_port(const char*);
/** @brief Initialize port (linux handling) */
int setup_port(int);
/** @brief Closes serial port and removes lock file */
int close_port(int);

#endif
