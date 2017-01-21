/**
 * Command line utility for testing serial connection to avr
 * in 6x pwm configuration.  Prompts for channel selection
 * and pwm value (0-255), reads serial response from avr.
 */

//#define _XOPEN_SOURCE 500

#include <iostream>
#include <ctime>
#include <cstdio>
#include <iomanip>
#include "serial_setup.h"
#include "crc.h"

void writeValue(uint8_t, int);
void writeCRC(uint8_t c[]); 
void printMessage(uint8_t c[], int size);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: ./SerialAutoTest [port]\n");
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
    uint8_t seed = 0x10;
    while (1) {
        writeValue(seed, fd);
	seed += 0x10;
	if (seed == 0xF0) seed = 0x10;
        //sleep(1);
        usleep(500000);
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
    short int sum = 0;
    uint8_t bytes[11]; // START (1) + motor (1*4) + servo (2*2) + CRC (2)
    memset(bytes, 0, 11);

    bytes[0] = 0xFF;
    bytes[1] = seed;
    bytes[2] = seed + 0x10;
    bytes[3] = seed + 0x20;
    bytes[4] = seed + 0x30;
    uint8_t servo = 0x00;
    if (seed > 0xA0) {
        servo = 0xFF;
    }

    bytes[5] = servo;
    bytes[6] = servo;
    bytes[7] = ~servo;
    bytes[8] = ~servo;
    
    // bytes[9], bytes[10] for crc

    for (int i = 1; i < 9; i++) {
        sum += bytes[i];
    }
    writeCRC(bytes);

    rc = write(fd, bytes, 11);
    if (rc < 0) {
        std::cerr << "Error on write: " << rc << std::endl;
    } else {
        std::cout << "Write success: " << std::endl;
        printMessage(bytes, 11);
    }
    tcdrain(fd);
    usleep(100000);
    memset(bytes, 0, 11);
    std::cout << "Waiting for response..." << std::endl;
    rc = read(fd, bytes, 11);
    if (rc == 11) {
        std::cout << "Reply:" << std::endl;
        printMessage(bytes, 11);
    } else {
        std::cerr << "Error on read: " << rc << std::endl;
        printMessage(bytes, 11);
    }
    std::cout << std::endl;
}

void writeCRC(uint8_t c[]) {
    uint16_t crc = calculate_crc16(c, 9);
    c[9] = (crc & 0xFF);
    c[10] = (crc >> 8);
}

void printMessage(uint8_t c[], int size) {
        std::cout << std::hex; 
        for (int i = 0; i < size; i++) {
            std::cout << std::uppercase << std::setfill('0') << std::setw(2) << (int)c[i] << " ";
        }
        std::cout << std::dec << std::endl;
}

