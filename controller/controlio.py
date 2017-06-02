from threading import Thread, Lock
import zmq
import struct
from time import sleep

class ControlIO(Thread):
    def __init__(self, host, port = 7500):
        Thread.__init__(self)
        self.host = host
        self.port = port
        self.rlock = Lock()
        self.vlock = Lock()
        self.ready = True
        self.voltage = [0.0, 0.0, 0.0, 0.0, 0.0]
        self.v_index = 0
        self.active = True
        self.nextRequest = None
        self.lastRequest = None

    def run(self):
        self.setup()
        while self.active:
            with self.rlock:
                request = self.nextRequest
            if request is not None:
                if self.nextRequest != self.lastRequest:
                    self.ready = False
                    self.socket.send(self.nextRequest)
                    try:
                        reply = self.socket.recv()
                        with self.vlock:
                            self.voltage[self.v_index]  = struct.unpack("<f", reply)[0]
                            self.v_index = (self.v_index + 1) % 5
                        self.lastRequest = self.nextRequest
                        self.nextRequest = None
                    except:
                        print("Error in recv block")
                    self.ready = True
            else:
                sleep(0.25)
        self.cleanup()

    def setup(self):
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REQ)
        self.socket.RCVTIMEO = 3000
        self.socket.connect("tcp://%s:%d" % (self.host, self.port))

    def cleanup(self):
        self.socket.close()
        self.context.destroy()

    def send_command(self, clist):
        with self.rlock:
            self.nextRequest = struct.pack("<iiii", *clist)

    def get_voltage(self):
        with self.vlock:
            return sum(self.voltage) / 5.0

    def is_ready(self):
        return self.ready
