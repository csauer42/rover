#include "crc.h"

uint16_t crc16_update(uint16_t crc, uint8_t a) {
    int i;
    crc ^= a;
    for (i = 0; i < 8; ++i) {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }
    return crc;
}

uint16_t calculate_crc16(uint8_t data[], int size) {
    int i;
    uint16_t crc = CRC16INIT;
    for (i = 0; i < size; i++) {
        crc = crc16_update(crc, data[i]);
    }
    return crc;
}

uint8_t crc_ibutton_update(uint8_t crc, uint8_t data) {
    uint8_t i;
    crc ^= data;
    for (i = 0; i < 8; i++) {
        if (crc & 0x01)
            crc = (crc >> 1) ^ 0x8C;
        else
            crc >>= 1;
    }
    return crc;
}

uint8_t calculate_crc8(uint8_t data[], int size) {
    int i;
    uint8_t crc = CRC8INIT;
    for (i = 0; i < size; i++) {
        crc = crc_ibutton_update(crc, data[i]);
    }
    return crc;
}
