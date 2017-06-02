#!/usr/bin/python3

import zmq
import struct

sock = 7500
ip = '192.168.0.104'

context = zmq.Context()

socket = context.socket(zmq.REQ)
socket.connect("tcp://%s:%d" % (ip, sock))

while True:
    try:
        values = list(map(int, input("Enter 4 values: ").split()))
        if len(values) != 4:
            print("Error, try again")
            continue
        request = struct.pack("<iiii", *values)
        print("Sending: %s" % str(values))
        socket.send(request)
        reply = socket.recv()
        print("Return value: %0.2f" % struct.unpack("<f", reply)[0])
    except KeyboardInterrupt:
        break

print()
socket.close()
