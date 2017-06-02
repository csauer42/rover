from threading import Thread, Lock
import cv2
import socket
import numpy as np
import pygame
import os

class VideoReceiver(Thread):
    def __init__(self, host, port = 5001):
        Thread.__init__(self)
        self.host = host
        self.lock = Lock()
        self.framebuffer = pygame.Surface((640,480))
        self.active = True
        self.buff = b''
        self.BUFF_SIZE = 1024
        self.port = port
        self.takeSnapshot = False
        self.recordVideo = 0
        self.path = "output"
        self.snap_name = [self.path + "/screenshot%d.png", 1]
        self.vid_name = [self.path + "/video%d.avi", 1]

    def run(self):
        self.setup()
        while self.active:
            start = self.buff.find(b'\xff\xd8')
            stop = self.buff.find(b'\xff\xd9')
            if start != -1 and stop != -1:
                frame = self.buff[start:stop+2]
                self.buff = self.buff[stop+2:]
                if len(frame) > 0:
                    image = cv2.imdecode(np.fromstring(frame, dtype=np.uint8), cv2.IMREAD_COLOR)
                    if self.takeSnapshot:
                        cv2.imwrite(self.snap_name[0] % self.snap_name[1], image)
                        self.snap_name[1] += 1
                        self.takeSnapshot = False
                    frameSize = image.shape[1::-1]
                    rgbframe = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
                    pygameFrame = pygame.image.frombuffer(rgbframe.tostring(), frameSize, 'RGB')
                    with self.lock:
                        self.framebuffer = pygameFrame
            else:
                self.buff += self.client.recv(self.BUFF_SIZE)
        self.cleanup()

    def setup(self):
        if not os.path.exists(self.path):
            os.makedirs(self.path)
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect((self.host, self.port))
        self.buff += self.client.recv(self.BUFF_SIZE)

    def get_frame(self):
        with self.lock:
            return self.framebuffer

    def cleanup(self):
        self.client.close()

    def snapshot(self):
        self.takeSnapshot = True

    def toggleVideo(self):
        self.recordVideo ^= 1
