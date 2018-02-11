#include "serial_comms.h"

float writeCommand(uint8_t c[], int fd) {
    int rc;
    uint8_t vret[2];
    tcflush(fd, TCIFLUSH);

    rc = write(fd, c, CLENGTH);
    if (rc < 0) {
        std::cerr << "Error on write: " << rc << std::endl;
    }

    tcdrain(fd);
    usleep(100000); // 100ms

    memset(vret, 0, 2);
    memset(c, 0, CLENGTH);

    rc = read(fd, vret, 2);
    if (rc == 2) {
        //good voltage return
        // 10-bit adc w/ .5 voltage divider
        // 0-1023 -> 0 - 10.0 volts
        return 10.0 * ((vret[0]<<8)|vret[1]) / 1023.0;
    } else {
        std::cerr << "Error on return: " << rc << std::endl;
        return -1.0f;
    }
}
