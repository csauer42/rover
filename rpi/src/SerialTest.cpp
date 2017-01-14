/**
 * Command line utility for testing serial connection to avr
 * in 6x pwm configuration.  Prompts for channel selection
 * and pwm value (0-255), reads serial response from avr.
 */

//#define _XOPEN_SOURCE 500

#include <iostream>
#include "serial_setup.h"

int getChannel();
int getValue();
void writeValue(int, int, int);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: ./SerialTest [port]\n");
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
    while (1) {
        int channel = getChannel();
        int value = getValue();
        writeValue(channel, value, fd);
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

int getChannel() {
    int channel;
    std::cout << std::endl << "Select pwm channel:" << std::endl <<
                              "1) OC0A" << std::endl <<
                              "2) OC0B" << std::endl <<
                              "3) OC1A" << std::endl <<
                              "4) OC1B" << std::endl <<
                              "5) OC2A" << std::endl <<
                              "6) OC2B" << std::endl <<
                              "? ";
    std::cin >> channel;
    return channel;
}

int getValue() {
    int value;
    std::cout << std::endl << "Input value (0-255): ";
    std::cin >> value;
    return value;
}

void writeValue(int channel, int value, int fd) {
    int rc;
    unsigned char bytes[6];
    memset(bytes, 0, 6);
    memset(bytes, 0, 1);
    std::cout << std::endl << "Writing value " << value << " to channel " << channel << std::endl;
    bytes[channel-1] = (value & 0xFF);
    rc = write(fd, bytes, 6);
    if (rc < 0) {
        std::cerr << "Error on write: " << rc << std::endl;
    } else {
        std::cout << "Write success: " << rc << std::endl;
    }
    std::cout << "Waiting for response..." << std::endl;
    rc = read(fd, bytes, 1);
    if (rc >= 1) {
        std::cout << "Reply: " << (int)bytes[0] << std::endl;
    } else {
        std::cerr << "Error on read: " << rc << std::endl;
    }
}
