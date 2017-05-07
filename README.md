# Mini-Rover Project

ARM/AVR-based rover with video feed, controlled via USB gamepad analog sticks.  Raspberry Pi Zero with
RedBear IoT pHat and PiCamera handles wifi, video, and command conversion.  ATMega328P connected via
UART for DC/servo motor control (PWM outputs) and battery voltage monitor.  Camera mounted on micro-servo
pan-tilt module for independent look control (+/- 90 degrees vertical and horizontal from straight ahead).
Powered by 7 AA NiMH batteries providing 8.4 volts to DC motors via L293 H-bridge and 5 volts to the rest 
of the circuit using a LM2575T-5.0 1A switching regulator.

Code:

avr/

Atmel Studio 7 project for ATMega328P motor control code.  Converts serial message
to motor control values on PWM pins, reads battery voltage with ADC pin.

controller/

Python application for converting gamepad input to 4 integer values, trasmitted
over ZeroMQ REQ/REP socket to Raspberry Pi.  Display raw h.264 feed from Pi and
battery voltage from AVR. 

rpi/

1) ZeroMQ<->UART interface, converts 4-integer values to 11-byte UART command w/ CRC, replies 
with voltage.  
2) raspivid piped to netcat for raw h.264 video stream using hardware encoder

Electrical schematics made with gEDA.

Dependencies:
python, evdev, ZeroMQ, avr-libc
