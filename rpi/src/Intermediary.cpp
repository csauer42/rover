#include "Rover.h"
#include <csignal>

void signalHandler(int);
Rover *rptr;

int main() {
    const char *tty = "/dev/ttyAMA0";
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    Rover rover = Rover(tty);
    rptr = &rover;
    rover.run();
    return 0;
}

void signalHandler(int signum) {
    rptr->stop();
}

