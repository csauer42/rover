#include "conversion.h"

void convertValues(int *input, uint8_t c[]) {
    c[0] = 0xFF;
    bool turnOnly = false;

    int motor_index = 0, turn_transform = 0;
    if (input[0] > 0) {
        motor_index = 1;
    } else if (input[0] < 0) {
        motor_index = 3;
    } else { //turn in position
        turnOnly = true;
    }

    if (!turnOnly) {
        c[motor_index] = std::abs(input[0]);
        c[motor_index+1] = std::abs(input[0]);

        if (input[1] < 0) {
            turn_transform = 1;
        }
        // decrease motor on turn side proportionately to induce turn
        c[motor_index + turn_transform] *= (1 - std::abs(input[1])/MAXMOTOR);
     } else {
        if (input[1] > 0) { //turn left
            c[2] = std::abs(input[1]);
            c[3] = std::abs(input[1]);
        } else { // turn right
            c[1] = std::abs(input[1]);
            c[4] = std::abs(input[1]);
        }
    }

    input[2] += LOOK_OFFSET;
    input[3] += LOOK_OFFSET;
    c[5] = input[2] >> 8;
    c[6] = input[2] & 0xFF;

    c[7] = input[3] >> 8;
    c[8] = input[3] & 0xFF;

    filterCommand(c);

    // apply crc to command
    uint16_t crc = calculate_crc16(c, CLENGTH-2);
    c[9] = crc & 0xFF;
    c[10] = crc >> 8;
}



void filterCommand(uint8_t c[]) {
    for (int i = 1; i < CLENGTH; i++) {
        if (c[i] == FILTER_BYTE) {
            c[i]--;
        }
    }
}
