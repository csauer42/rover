#!/usr/bin/python

import cv2
import socket
import numpy as np

HOST = '192.168.0.104'
PORT = 5001
BUFF_SIZE = 1024

buff = b''
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((HOST,PORT))

buff += client.recv(BUFF_SIZE)

while True:
    start = buff.find(b'\xff\xd8')
    stop = buff.find(b'\xff\xd9')
    if start != -1 and stop != -1:
        frame = buff[start:stop+2]
        buff = buff[stop+2:]

        image = cv2.imdecode(np.fromstring(frame, dtype=np.uint8), cv2.IMREAD_COLOR)
        cv2.imshow('frame', image)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    else:
        try:
            buff += client.recv(BUFF_SIZE)
        except KeyboardInterrupt:
            break

client.close()
cv2.destroyAllWindows()
