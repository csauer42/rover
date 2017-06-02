#!/usr/bin/python3

import zmq
import struct
from jsconverter import JSConverter
from time import sleep

sock = 7500
ip = '192.168.0.104'

#context = zmq.Context()

#socket = context.socket(zmq.REQ)
#socket.connect("tcp://%s:%d" % (ip, sock))

device = '/dev/input/event23'

jsc = JSConverter(device)

jsc.start()
previous = [0, 0, 0, 0]

while True:
    try:
        values = jsc.getStatus()
        if values != previous:
            print(values)
            request = struct.pack("<iiii", *values)
            print("Sending: %s" % str(values))
            #socket.send(request)
            #reply = socket.recv()
            previous = values
            #print("Return value: %0.2f" % struct.unpack("<f", reply)[0])
            #sleep(0.25)
    except KeyboardInterrupt:
        break

print()
jsc.active = False
#socket.close()
jsc.join()
print("OK")
