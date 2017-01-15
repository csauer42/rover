#!/usr/bin/python

from evdev import InputDevice, categorize, ecodes

gamepad = InputDevice('/dev/input/event16')
valid_codes = [0,1,3,4]
#stick = ["L-horizontal", "L-vertical", "PLACEHOLDER", "R-horizontal", "R-vertical"]
directions_h = ["left", "right"]
directions_v = ["forward", "backward"]

def formatEvent(code, val):
    if code < 3:
        stick_name = "Left"
    else:
        stick_name = "Right"
    if code == 1 or code == 4:
        direction = directions_v
    else:
        direction = directions_h
    d_key = 1
    if val < 0:
        d_key = 0
    print("[%s]: %s %d%%" % (stick_name, direction[d_key], abs((val/2**15)*100)))


for event in gamepad.read_loop():
    if event.type == 3 and event.code in valid_codes:
        formatEvent(event.code, event.value)
