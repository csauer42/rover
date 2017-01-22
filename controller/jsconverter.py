from evdev import InputDevice, categorize, ecodes
from threading import Thread

class JSConverter(Thread):
    MLIMITLOW = 32
    LLIMITLOW = 96
    MLIMITHIGH = 255
    LLIMITHIGH = 512
    VALID_CODES = [0,1,3,4]

    def __init__(self, device):
        Thread.__init__(self)
        self.gamepad = InputDevice(device)
        self.movement = [0,0] # [0] = l(+)/r(-), [1] = f(+)/b(-), 
        self.look = [0,0] # [0] = l/r [1] = u/d
        self.active = True

    def run(self):
        while self.active:
            for event in self.gamepad.read_loop():
                if not self.active:
                    break
                if event.type == 3 and event.code in self.VALID_CODES:
                    self.parseEvent(event.code, event.value)

    def parseEvent(self, code, val):
        if code < 3:
            self.movement[code] = self.movementFilter((val*-1) >> 7)
        else:
            self.look[code-3] = self.lookFilter((val*-1) >> 6)

    def getStatus(self):
        return [self.movement[1], self.movement[0], self.look[1], self.look[0]]

    def getStatusString(self):
        return ("Move:[%3d,%3d] | Look:[%4d,%4d]" % 
             (self.movement[0], self.movement[1], self.look[0], self.look[1]))

    def movementFilter(self, value):
        if value < self.MLIMITLOW and value > -1*self.MLIMITLOW:
            return 0
        if abs(value) >= self.MLIMITHIGH:
            if value > 0:
                return self.MLIMITHIGH
            return -1*(self.MLIMITHIGH-1)
        return value

    def lookFilter(self, value):
        if value < self.LLIMITLOW and value > -1*self.LLIMITLOW:
            return 0
        if abs(value) >= self.LLIMITHIGH:
            if value > 0:
                return self.LLIMITHIGH
            return -1*(self.LLIMITHIGH-1)
        return value
