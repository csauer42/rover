from threading import Thread, Lock
import cv2
import socket
import numpy as np
import pygame
import os

# to do: video recording
# alternate video: write raw frames as binary data w/o conversion

class VideoReceiver(Thread):
    """Receives and processes raw video stream from RPi"""
    FPS = 24
    SCREEN_SIZE = (640,480)

    def __init__(self, host, port = 5001):
        Thread.__init__(self)
        self.host = host
        self.lock = Lock()
        self.framebuffer = pygame.Surface((640,480))
        self.active = True
        self.buff = b''
        self.buffsize = 1024
        self.port = port
        self.takeSnapshot = False
        self.recordVideo = False
        self.path = "output"
        self.snap_name = [self.path + "/screenshot%d.png", 1]
        self.vid_name = [self.path + "/video%d.avi", 1]
        self.video_written = False

    def run(self):
        """Main VideoReceiver loop"""
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
                    if self.recordVideo:
                        self.vout.write(image) 
                    frameSize = image.shape[1::-1]
                    rgbframe = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
                    pygameFrame = pygame.image.frombuffer(rgbframe.tostring(), frameSize, 'RGB')
                    with self.lock:
                        self.framebuffer = pygameFrame
            else:
                self.buff += self.client.recv(self.buffsize)
        self.cleanup()

    def setup(self):
        """Create file paths, setup up receiver socket, do initial buffer read"""
        # create output directory as needed
        if not os.path.exists(self.path):
            os.makedirs(self.path)
        # check for existing screenshots, set id number
        # check for existing videos, set id number
        max_screen = 0
        max_vid = 0
        for f in os.listdir(self.path):
            if f[-3:] == "png":
                num = int(f[10:-4])
                if num > max_screen:
                    max_screen = num
            if f[-3:] == "avi":
                num = int(f[5:-4])
                if num > max_vid:
                    max_vid = num
        self.snap_name[1] = max_screen + 1
        self.vid_name[1] = max_vid + 1

        # open file for writing video.  each run will open a new file
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        self.vout = cv2.VideoWriter(self.vid_name[0] % self.vid_name[1], fourcc, self.FPS, 
                self.SCREEN_SIZE)
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect((self.host, self.port))
        self.buff += self.client.recv(self.buffsize)

    def get_frame(self):
        """Returns frame to calling process"""
        with self.lock:
            return self.framebuffer

    def cleanup(self):
        """Closes socket connection, cleans up video file"""
        self.client.close()
        if not self.video_written:
            os.remove(self.vid_name[0] % self.vid_name[1])

    def snapshot(self):
        """Set flag to record next frame as png snapshot"""
        self.takeSnapshot = True

    def toggleVideo(self):
        """Toggle video recording (not yet implemented)"""
        if not self.video_written:
            self.video_written = True
        self.recordVideo = not self.recordVideo
