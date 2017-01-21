#include "Rover.h"

Rover::Rover(const char *ttydevice) {
    runflag = true;
    // setup serial port
    serialfd = open_port(ttydevice);
    int rc = setup_port(serialfd);
    if (rc < 0) {
       // raise exception
    }
    // open zmq socket
    ctx = new zmq::context_t(1);
    socket = new zmq::socket_t(*ctx, ZMQ_REP);
    // set socket parameters
    socket->bind("tcp://*:7500");
}

Rover::~Rover() {
    close_port(serialfd);
    socket->close();
    delete(socket);
    delete(ctx);
}

void Rover::run() {
    while (runflag) {
        float voltageReturn = -2.0f; // request error
        zmq::message_t request;
        try {
            socket->recv(&request);
        } catch (zmq::error_t &e) {
            if (!runflag) break;
        }
        if (request.size()/sizeof(int) == 4) {
            convertValues((int*)request.data(), command);
///////// start debug
            for (int i = 0; i < CLENGTH; i++) {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)command[i] << " ";
            }
            std::cout << std::dec << std::endl;
///////// end debug
            voltageReturn = writeCommand(command, serialfd); // reply error or voltage
///////// start debug
            std::cout << "Voltage returned: " << voltageReturn << std::endl;
///////// end debug            
        }
        zmq::message_t response(4);
        memcpy(response.data(), &voltageReturn, sizeof(float));
        socket->send(response);
    }
}

void Rover::stop() {
    runflag = false;
}
