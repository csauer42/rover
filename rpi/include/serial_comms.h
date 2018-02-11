#ifndef SERIAL_COMMS_H
#define SERIAL_COMMS_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <termios.h>
#include <unistd.h>
#include <ctime>
#include <cstdint>
#include "constants.h"

/** writes command to serial device and reads reply */
float writeCommand(uint8_t[], int);

#endif
