# Mini-Rover Project

ARM/AVR-based rover with video feed, controlled via USB gamepad analog sticks.  Raspberry Pi Zero with
RedBear IoT pHat (which could be replaced by a Raspberry Pi Zero W) and PiCamera handles wifi, video, and 
command conversion.  ATMega328P connected via UART for DC/servo motor control (PWM outputs) and battery 
voltage monitor.  Camera mounted on micro-servo pan-tilt module for independent look control (+/- 90 
degrees vertical and horizontal from straight ahead). Powered by 7 AA NiMH batteries providing ~8.4 volts 
to DC motors via L293 H-bridge and 5 volts to the rest of the circuit using a LM2575T-5.0 1A switching regulator.

![Rover side view](http://i.imgur.com/s8KuVWz.jpg =150px) ![Rover front view](http://i.imgur.com/gCrFFxG.jpg =150px)

## Repository Contents
(additional notes are provided in each directory)

### avr

Atmel Studio 7 project for ATMega328P motor control code.  Converts serial message
to motor control values on PWM pins.  Also reads battery voltage with ADC pin.

### controller

Python application for converting gamepad input to 4 integer values, trasmitted
over ZeroMQ REQ/REP socket to Raspberry Pi.  Display MPEG feed from Pi Camera and
battery voltage from AVR.  Allows snapshots and video recording on controller device. 
OpenCV decoding and processing allow general computer-vision additions before frame 
is provided to display thread.

### rpi

1. ZeroMQ<->UART interface, converts 4-integer values to 11-byte UART command w/ CRC, replies 
with voltage.  
2.  /dev/video0 (RPi Camera configured with l4v2-ctl) piped to netcat for MPEG video stream.

### schematics

MotorControl board handles interfacing RPi control of dc and servo motors through AVR via PWM outputs, 
voltage conversion from battery to 5v, and level shifting between 3.3V and 5V for UART communication 
between AVR and RPi. 

Electrical schematics and pcb layout files made with [gEDA](http://gedaproject.org). Board ordered from [OSH Park](http://oshpark.com).

Version 1 tested and functional, though some holes should be enlarged in version 2 (LM2575, 4-pin terminal block, 
L293D, 1N5819).

## Project Dependencies:
* [python3](https://www.python.org)
* [pygame](https://www.pygame.org)
* [opencv](http://opencv.org)
* [numpy](http://numpy.org)
* [ZeroMQ](http://zeromq.org)
* [avr-libc](http://www.nongnu.org/avr-libc/)
* [v4l2-utils](https://github.com/gjasny/v4l-utils) (for v4l2-ctl)

## Notes

* For general use, RPi can run on a local network.  As long as the controlling computer is on the same 
network, it's just a matter of connecting to the correct IP.  For portability, RPi should be set up as
a network access point, and controlling computer should connect to that network.  The control program 
can then connect to the network gateway.

* Since gEDA appears to have a limited user base, future revisions of the MotorControl board will be done
in Eagle or KiCad.

* Current version of controller program requires gamepad input to function.  Future version to include 
keyboard control as fallback/secondary input.
