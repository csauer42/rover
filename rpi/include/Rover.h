#ifndef ROVER_H
#define ROVER_H

// serial
// crc
// conversion

#include <iostream>
#include <iomanip> ///// debug
#include <cstdint>
#include <cstdlib>
#include <zmq.hpp>

#include "serial_setup.h"
#include "conversion.h"
#include "serial_comms.h"

class Rover {
public:
    Rover(const char *ttydevice);
    ~Rover();
    void run();
    void stop();
private:
    bool runflag;
    int serialfd;
    zmq::context_t *ctx;
    zmq::socket_t *socket;
    uint8_t command[CLENGTH]; 
};

#endif
