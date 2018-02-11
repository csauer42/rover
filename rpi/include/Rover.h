#ifndef ROVER_H
#define ROVER_H

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <zmq.hpp>

#include "serial_setup.h"
#include "conversion.h"
#include "serial_comms.h"

/** Rover Class, handles tranfer and conversion 
    of control values and voltage reads */
class Rover {
public:
    /** Constructor takes serial port name */
    Rover(const char *ttydevice);
    ~Rover();
    void run();
    void stop();
private:
    bool runflag;
    int serialfd;
    zmq::context_t *ctx;
    zmq::socket_t *socket;
    uint8_t command[CLENGTH]; // from constants.h
};

#endif
