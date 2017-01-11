#!/usr/bin/python3

import zmq
import struct
import random

random.seed()

sock = 7500
ip = '172.16.248.87'

context = zmq.Context()

socket = context.socket(zmq.REQ)
socket.connect("tcp://%s:%d" % (ip, sock))

values = []

for i in range(6):
    values.append(random.randint(0,255))

#values = [ 10, 20, 30, 40, 50, 60 ]
# add crc calculation as last byte(s)

request = struct.pack(">BBBBBB", *values)

print("Sending: %s" % str(values))

socket.send(request)

reply = socket.recv()

return_value = struct.unpack(">B", reply)[0]

print("Received: %.2f" % (struct.unpack(">B", reply)[0]/100.0))

socket.close()
