#include "Rover.h"
#include <csignal>

void signalHandler(int);

Rover *rptr;

int main() {
    const char *tty = "/dev/ttyAMA0";
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    try {
        Rover rover = Rover(tty);
        rptr = &rover;
        rover.run();
    } catch (std::runtime_error &e) {
        std::cerr << "[SERIAL ERROR] " << e.what() << std::endl;
        return 1;
    } catch (zmq::error_t &e) {
        std::cerr << "[ZMQ ERROR] " << e.what() << std::endl;
        return 2;
    }
    return 0;
}

void signalHandler(int signum) {
    rptr->stop();
}

