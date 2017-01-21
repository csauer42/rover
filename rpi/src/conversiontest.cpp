#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>

#define MAXMOTOR 255.0
#define FILTER_BYTE 0xFF
#define CLENGTH 11
#define LOOK_OFFSET 511

std::vector<int> getValues();
void convertValues(std::vector<int>, uint8_t c[]);
void filterCommand(uint8_t c[]);
void printCommand(uint8_t c[]);

int main() {
    // motor range: -254 to 255
    // look range:  -511 to 512
    // filter byte == 0xFF
    uint8_t command[CLENGTH];

    for (;;) {
        memset(command, 0, CLENGTH);
        std::vector<int> values = getValues();
        convertValues(values, command);
        printCommand(command);
    }
    return 0;
}

std::vector<int> getValues() {
    std::vector<int> values;
    // values: 1 = MF | 2 = MT | 3 = LV | 4 = LH
    int mf, mt, lv, lh;

    std::cout << "Enter 4-val position (MF/T,LV/H): ";
    std::cin >> mf >> mt >> lv >> lh;
    std::cout << "motorForward: " << mf << std::endl
              << "motorTurn: " << mt << std::endl
              << "lookVert: " << lv << std::endl
              << "lookHoriz: " << lh << std::endl;
    values.push_back(mf);
    values.push_back(mt);
    values.push_back(lv);
    values.push_back(lh);

    return values;
}

void convertValues(std::vector<int> input, uint8_t c[]) {
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
        c[motor_index] = abs(input[0]);
        c[motor_index+1] = abs(input[0]);

        if (input[1] < 0) {
            turn_transform = 1;
        }
        // decrease motor on turn side proportionately to induce turn
        c[motor_index + turn_transform] *= (1 - abs(input[1])/MAXMOTOR);
     } else {
        if (input[1] > 0) { //turn left
            c[2] = abs(input[1]);
            c[3] = abs(input[1]);
        } else { // turn right
            c[1] = abs(input[1]);
            c[4] = abs(input[1]);
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
    c[9] = 0xAA; //dummy value
    c[10] = 0xAA;
}

void filterCommand(uint8_t c[]) {
    for (int i = 1; i < CLENGTH; i++) {
        if (c[i] == FILTER_BYTE) {
            c[i]--;
        } 
    }
}

void printCommand(uint8_t c[]) {
    std::cout << "Command:" << std::endl;
    for (int i = 0; i < CLENGTH; i++) {
        std::cout << std::hex << std::uppercase << std::setfill('0') 
                  << std::setw(2) << (int)c[i] << " ";
    }
    std::cout << std::dec << std::endl << std::endl;
}

