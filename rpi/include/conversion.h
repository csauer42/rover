#ifndef CONVERSION_H
#define CONVERSION_H

#include <cmath>
#include "constants.h"
#include "crc.h"

/** Convert 4 integer motor values to 11-byte serial message */
void convertValues(int*, uint8_t c[]);
/** Remove any 0xFF byte from command before applying CRC */
void filterCommand(uint8_t c[]);

#endif

