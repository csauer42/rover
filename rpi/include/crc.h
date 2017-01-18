#ifndef CRC_H
#define CRC_H

#define CRC16INIT 0xFFFF
#define CRC8INIT 0x00
#include <cstdint>


// crc implementations borrowed from <util/crc16.h>
uint16_t crc16_update(uint16_t, uint8_t);
uint16_t calculate_crc16(unsigned char[], int size);
uint8_t crc_ibutton_update(uint8_t crc, uint8_t data);
uint8_t calculate_crc8(uint8_t[], int);

#endif
