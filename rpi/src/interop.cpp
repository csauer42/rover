#include <iostream>
#include <zmq.hpp>

void printValues(int*, int);

int main() {
    zmq::context_t ctx(1);
    zmq::socket_t socket(ctx, ZMQ_REP);
    socket.bind("tcp://*:7500");
    while (1) {
        zmq::message_t req;
        socket.recv(&req);
        printValues((int*)req.data(), req.size()/sizeof(int));
        float test = 7.1;
        zmq::message_t rep(4);
        memcpy(rep.data(), &test, sizeof(float));
        socket.send(rep);
    }

    return 0;
}

void printValues(int *r, int l) {
    for (int i = 0; i < l; i++) {
        std::cout << "| " << (int)r[i] << " ";
    }
    std::cout << "|" << std::endl;
}
