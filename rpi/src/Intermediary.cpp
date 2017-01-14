#include <zmq.hpp>
#include <cstdio>

#define REQ_LENGTH 6

void parseRequest(uint8_t*, int);

int main() {
    uint8_t counter = 0;
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:7500");

    while (true) {
        zmq::message_t request;
        socket.recv(&request);
        parseRequest((uint8_t*)request.data(), request.size());
        zmq::message_t response(1);
        memcpy(response.data(), &counter, 1);
        counter++;
        printf("Counter incremented to %d\n\n", counter);
        socket.send(response); 
    }


    return 0;
}

void parseRequest(uint8_t *r, int s) {
    if (s != REQ_LENGTH) {
        printf("parseRequest() failed because of invalid argument length: %d\n", s);
        return;
    }
    printf("Valid request received:\n");
    for (int i = 0; i < REQ_LENGTH; i++) {
        printf("Arg[%d]: %d\n", i, r[i]);
    }
}

