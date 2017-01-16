/**
 * Command line utility for testing serial connection to avr
 * in 6x pwm configuration.  Prompts for channel selection
 * and pwm value (0-255), reads serial response from avr.
 */

//#define _XOPEN_SOURCE 500

#include <iostream>
#include <ctime>
#include "serial_setup.h"

void writeValue(uint8_t, int);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: ./SerialReqRep [port]\n");
        return -1;
    }
    // begin serial port setup

    int fd, rc;
    fd = open_port(argv[1]);

    if (fd < 0) {
        switch (fd) {
            case ERRLOPEN:
                std::cerr << "Error opening lock file" << std::endl;
                break;
            case ERRLLOCK:
                std::cerr << "Error locking file" << std::endl;
                break;
            case ERRPOPEN:
                std::cerr << "Error opening port" << std::endl;
                break;
            case ERRPFCNTL:
                std::cerr << "Error running fcntl" << std::endl;
                break;
            case ERRPIOCTL:
                std::cerr << "Error running ioctl" << std::endl;
                break;
            default:
                std::cerr << "Unknown error during open_port" << std::endl;
        }
        return -1; 
    }

    rc = setup_port(fd); 
    if (rc < 0) {
        switch (rc) {
            case ERRPFLUSH:
                std::cerr << "Error flushing port" << std::endl;
                break;
            case ERRPSETATTR:
                std::cerr << "Error setting port attributes" << std::endl;
                break;
            default:
                std::cerr << "Unknown error during setup_port" << std::endl;
        }
        std::cerr << "Failed to setup serial port.  Exiting." << std::endl;
        close_port(fd);
        return -1; 
    }
    // end serial port setup

    // begin main process
    uint8_t seed = 0x01;
    while (1) {
        writeValue(seed, fd);
	seed++;
	if (seed == 0xFE) seed = 0x01;
        usleep(50000);
    }
    // end main process

    // cleanup
    rc = close_port(fd);
    if (rc < 0) {
        if (rc & ERRPCLOSE)
            std::cerr << "Error closing port." << std::endl;
        if (rc & ERRLUNLOCK)
            std::cerr << "Error unlocking file." << std::endl;
        if (rc & ERRLREMOVE)
            std::cerr << "Error removing file." << std::endl;
    }
    return 0;
}

void writeValue(uint8_t seed, int fd) {
    int rc;
    unsigned char bytes[1];
    memset(bytes, 0, 1);

    bytes[0] = seed;

    rc = write(fd, bytes, 1);
    if (rc < 0) {
        std::cerr << "Error on write: " << rc << std::endl;
        return;
    } else {
        std::cout << "Write success: " << rc << " Sent: " << (int)seed << std::endl;
    }
    tcdrain(fd);
    std::cout << "Waiting for response..." << std::endl;
    rc = read(fd, bytes, 1);
    if (rc >= 1) {
        std::cout << "Reply: " << (int)bytes[0] << " Expected reply: " << seed+1 << std::endl << std::endl;
    } else {
        std::cerr << "Error on read: " << rc << std::endl << std::endl;
    }
}
