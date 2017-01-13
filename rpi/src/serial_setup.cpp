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

    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
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
    struct termios tty;
    tty.c_cflag = TTYBAUDRATE | CS8 | CLOCAL | CREAD;
    tty.c_iflag = IGNPAR | ICRNL;
    tty.c_oflag = 0;
    tty.c_lflag = ICANON;

    tty.c_cc[VINTR]    = 0;     /* Ctrl-c */
    tty.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    tty.c_cc[VERASE]   = 0;     /* del */
    tty.c_cc[VKILL]    = 0;     /* @ */
    tty.c_cc[VEOF]     = 4;     /* Ctrl-d */
    tty.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    tty.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    tty.c_cc[VSWTC]    = 0;     /* '\0' */
    tty.c_cc[VSTART]   = 0;     /* Ctrl-q */
    tty.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    tty.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    tty.c_cc[VEOL]     = 0;     /* '\0' */
    tty.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    tty.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    tty.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    tty.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    tty.c_cc[VEOL2]    = 0;     /* '\0' */

    if (tcflush(fd, TCIFLUSH)) {
        return ERRPFLUSH;
    }
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
