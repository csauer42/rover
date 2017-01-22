#!/usr/bin/python3

from jsconverter import JSConverter
from time import sleep

device = '/dev/input/event22'

jsc = JSConverter(device)

jsc.start()
last_status = ""

while True:
    try:
        status = jsc.getStatusString()
        if status != last_status:
            print(status)
            last_status = status
        sleep(0.1)
    except KeyboardInterrupt:
        break

jsc.active = False
jsc.join() # run loop won't quit until it receives another event

print("OK")
