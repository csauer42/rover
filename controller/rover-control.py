#!/usr/bin/python

import pygame
from pygame.locals import *
from threading import Lock
from videoreceiver import VideoReceiver
from controlio import ControlIO
import sys

def js_convert(pos_list):
    MLIMITLOW = 32
    LLIMITLOW = 96
    c0 = int(pos_list[0] * -255)
    c1 = int(pos_list[1] * 255)
    c2 = int(pos_list[2] * 512)
    c3 = int(pos_list[3] * -512)
    if c0 < MLIMITLOW and c0 > -1 * MLIMITLOW:
        c0 = 0
    if c1 < MLIMITLOW and c1 > -1 * MLIMITLOW:
        c1 = 0
    if c2 < LLIMITLOW and c2 > -1 * LLIMITLOW:
        c2 = 0
    if c3 < LLIMITLOW and c3 > -1 * LLIMITLOW:
        c3 = 0
    return (c0, c1, c2, c3) 

def text_objects(text, font):
    textSurface = font.render(text, True, (255,255,255))
    return textSurface, textSurface.get_rect()

def gamepad_position(clist, screen):
    font = pygame.font.Font('freesansbold.ttf', 16)
    TextSurf, TextRect = text_objects("Move [%4d,%4d]    Look[%4d,%4d]" % clist, font)
    TextRect.center = (320,400)
    screen.blit(TextSurf, TextRect)

def battery_voltage(voltage, screen):
    font = pygame.font.Font('freesansbold.ttf', 16)
    TextSurf, TextRect = text_objects("Voltage: %.2f" % voltage, font)
    TextRect.center = (75, 460)
    screen.blit(TextSurf, TextRect)

def main():
    BLACK = (0,0,0)
    RED = (255,0,0)
    run = True

    voltage = 8.75
    recording_video = False
    video_socket_connected = False
    control_socket_connected = False
    capture_frame = False

    if len(sys.argv) != 2:
        print("Usage: ./rover-control.py [ip address]")
        sys.exit(1)

    rover_address = sys.argv[1]
    pygame.init()

    pygame.joystick.init()
    clock = pygame.time.Clock()

    if pygame.joystick.get_count() > 0:
        gamepad = pygame.joystick.Joystick(0)
        gamepad.init()
    else:
        print("Gamepad not found.  Exiting.")
        sys.exit(2)

    screen = pygame.display.set_mode([640,480])

    pygame.display.set_caption("Rover Control")
    video_receiver = VideoReceiver(rover_address)
    video_receiver.start()

    cio = ControlIO(rover_address)
    cio.start()

    while run:
        for event in pygame.event.get():
            if event.type == QUIT:
                run = False
            elif event.type == JOYBUTTONDOWN:
                if event.button == 0:
                    print("Doing snapshot.")
                    video_receiver.snapshot()
                elif event.button == 1:
                    print("Doing video.")
                    video_receiver.toggleVideo()
        m0 = gamepad.get_axis(1)
        m1 = gamepad.get_axis(0)
        l0 = gamepad.get_axis(4)
        l1 = gamepad.get_axis(3)
        values = ( m0, m1, l0, l1 )
        screen.fill(BLACK)
        screen.blit(video_receiver.get_frame(), (0,0))
        gamepad_values = js_convert(values)
        if cio.is_ready():
            cio.send_command(gamepad_values)
        gamepad_position(gamepad_values, screen)
        battery_voltage(cio.get_voltage(), screen)
        pygame.display.flip()
        clock.tick(24)

    video_receiver.active = False
    cio.active = False
    video_receiver.join()
    cio.join()
    pygame.quit()
    

if __name__ == '__main__':
    main()

