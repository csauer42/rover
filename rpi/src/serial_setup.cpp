#include "serial_setup.h"

int lock_file;

int open_port(const char *port) {
    int fd, rc;
    lock_file = open(LOCKNAME, O_RDWR | O_CREAT, 0666);
    if (lock_file == -1) {
        return ERRLOPEN;
    }
    rc = flock(lock_file, LOCK_EX | LOCK_NB);
    if (rc == -1) {
        return ERRLLOCK;
    }

    fd = open(port, O_RDWR | O_NOCTTY | O_SYNC); // | O_NDELAY);
    if (fd == -1) {
        return ERRPOPEN;
    } else {
        rc = fcntl(fd, F_SETFL, 0);
        if (rc == -1) {
            return ERRPFCNTL;
        }
        rc = ioctl(fd, TIOCEXCL);
        if (rc == -1) {
            return ERRPIOCTL;
        }
    }

    return fd;
}

int setup_port(int fd) {
    // use cfmakeraw() ?
    struct termios tty;
 
    if (tcgetattr(fd, &tty) < 0) {
	return ERRPFLUSH;
    }

    cfsetospeed(&tty, (speed_t)TTYBAUDRATE);
    cfsetispeed(&tty, (speed_t)TTYBAUDRATE);

    tty.c_cflag |= (CLOCAL | CREAD); // unnecessary?
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    //tty.c_cflag &= ~CSTOPB; // defaults to off/covered by CS8
    tty.c_cflag &= ~CRTSCTS;

    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VTIME]    = 10;    // 1 second inter-character timer
    tty.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */

    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &tty)) {
        return ERRLUNLOCK;
    }

    return 0;
}

int close_port(int fd) {
    int rc = 0;

    if (close(fd))
        rc |= ERRPCLOSE;

    if (flock(lock_file, LOCK_UN))
        rc |= ERRLUNLOCK;

    if (remove(LOCKNAME))
        rc |= ERRLREMOVE;

    return rc;
}
