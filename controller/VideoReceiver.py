#!/usr/bin/python

import cv2
import socket
import numpy as np

HOST = '192.168.0.104'
PORT = 5001

vbytes = b''
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((HOST,PORT))

vbytes += client.recv(2048)

while True:
    a = vbytes.find(b'\xff\xd8')
    b = vbytes.find(b'\xff\xd9')
    if a != -1 and b != -1:
        jpg = vbytes[a:b+2]
        vbytes = vbytes[b+2:]

        i = cv2.imdecode(np.fromstring(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
        cv2.imshow('frame', i)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    else:
        try:
            vbytes += client.recv(2048)
        except KeyboardInterrupt:
            break

client.close()
cv2.destroyAllWindows()
