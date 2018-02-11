#include "Rover.h"

Rover::Rover(const char *ttydevice) {
    runflag = true;
    // setup serial port
    serialfd = open_port(ttydevice);
    int rc = setup_port(serialfd);
    if (rc < 0) {
       throw std::runtime_error("Failed to set up serial port");
    }
    // open zmq socket, will throw zmq::error_t if they fail
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
    float voltageReturn = 0.0f;

    while (runflag) {
        zmq::message_t request;
        try {
            socket->recv(&request);
        } catch (zmq::error_t &e) {
            if (!runflag) break;
        }
        if (request.size()/sizeof(int) == 4) {
            convertValues(static_cast<int*>(request.data()), command);
            voltageReturn = writeCommand(command, serialfd); // replies with voltage
        }
        zmq::message_t response(4);
        memcpy(response.data(), &voltageReturn, sizeof(float));
        socket->send(response);
    }
}

void Rover::stop() {
    runflag = false;
}
