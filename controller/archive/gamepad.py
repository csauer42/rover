#!/usr/bin/python

import pygame
from time import sleep

pygame.init()

js = pygame.joystick.Joystick(0)
js.init()

print(js.get_id())
print(js.get_name())

print()

clock = pygame.time.Clock()

while True:
    try:
        pygame.event.wait()
        for i in range(js.get_numaxes()):
            print("Axis %d:%d" % (i, js.get_axis(i)))
        for i in range(js.get_numbuttons()):
            print("Button %d:%d" % (i, js.get_button(i)))
        print("-"*20)
        clock.tick(100)
    except KeyboardInterrupt:
        break

pygame.quit()
 
