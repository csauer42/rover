import pygame
from pygame.locals import *
from threading import Lock
from videoreceiver import VideoReceiver
from controlio import ControlIO
import sys

class Rover(object):
    """Primary control interface for Rover"""
    FPS = 15
    MLIMITLOW = 32
    LLIMITLOW = 96
    WIDTH = 1280
    HEIGHT = 960
    BLACK = (0,0,0)
    WHITE = (255,255,255)
    RED = (255,0,0)

    def __init__(self, ip):
        self.stop = False
        self.ip = ip

    def setup(self):
        """Handles library initialization and additional thread setup"""
        pygame.init()
        pygame.joystick.init()
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font('freesansbold.ttf', 16)
        if pygame.joystick.get_count() > 0:
            self.gamepad = pygame.joystick.Joystick(0)
            self.gamepad.init()
        else:
            print("Gamepad not found.  Exiting.")
            sys.exit(2)
        self.screen = pygame.display.set_mode([self.WIDTH,self.HEIGHT])
    
        pygame.display.set_caption("Rover Control")
        self.video_receiver = VideoReceiver(self.ip)
        self.video_receiver.start()
    
        self.cio = ControlIO(self.ip)
        self.cio.start()

    def js_convert(self, pos_list):
        """Convert gamepad values from pygame format to motor control format"""
        c0 = int(pos_list[0] * -255)
        c1 = int(pos_list[1] * -255)
        c2 = int(pos_list[2] * 512)
        c3 = int(pos_list[3] * -512)
        if c0 < self.MLIMITLOW and c0 > -1 * self.MLIMITLOW:
            c0 = 0
        if c1 < self.MLIMITLOW and c1 > -1 * self.MLIMITLOW:
            c1 = 0
        if c2 < self.LLIMITLOW and c2 > -1 * self.LLIMITLOW:
            c2 = 0
        if c3 < self.LLIMITLOW and c3 > -1 * self.LLIMITLOW:
            c3 = 0
        return (c0, c1, c2, c3) 
    
    def text_objects(self, text):
        """Helper function for displaying text to screen"""
        textSurface = self.font.render(text, True, self.WHITE)
        return textSurface, textSurface.get_rect()
    
    def gamepad_position(self, clist):
        """Diplay gamepad analog stick positions to screen"""
        TextSurf, TextRect = self.text_objects("Move [%4d,%4d]    Look[%4d,%4d]" % clist)
        TextRect.center = (self.WIDTH-1000, self.HEIGHT-100)
        self.screen.blit(TextSurf, TextRect)
    
    def battery_voltage(self, voltage):
        """Display battery voltage to screen"""
        TextSurf, TextRect = self.text_objects("Voltage: %.2f" % voltage)
        TextRect.center = (self.WIDTH-280, self.HEIGHT-100)
        self.screen.blit(TextSurf, TextRect)
    
    def run(self):
        """Main control loop for rover"""
        self.setup()

        while not self.stop:
            m0 = self.gamepad.get_axis(1)
            m1 = self.gamepad.get_axis(0)
            l0 = self.gamepad.get_axis(4)
            l1 = self.gamepad.get_axis(3)

            values = ( m0, m1, l0, l1 )
            gamepad_values = self.js_convert(values)

            self.screen.fill(self.BLACK)
            self.screen.blit(self.video_receiver.get_frame(), (0,0))
            self.gamepad_position(gamepad_values)
            self.battery_voltage(self.cio.get_voltage())
            pygame.display.flip()

            if self.cio.is_ready():
                self.cio.send_command(gamepad_values)

            for event in pygame.event.get():
                if event.type == QUIT:
                    self.stop = True
                elif event.type == JOYBUTTONDOWN:
                    if event.button == 0:
                        print("Doing snapshot.")
                        self.video_receiver.snapshot()
                    elif event.button == 1:
                        print("Doing video.")
                        self.video_receiver.toggleVideo()
            self.clock.tick(self.FPS)
    
        self.video_receiver.active = False
        self.cio.active = False
        self.video_receiver.join()
        self.cio.join()
        pygame.quit()
