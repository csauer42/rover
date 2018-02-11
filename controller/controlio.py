from threading import Thread, Lock
import zmq
import struct
from time import sleep

class ControlIO(Thread):
    """Input/output of control codes and voltage readings between controller and rover"""
    VCOUNT = 5 # circular buffer size for voltage readings
    TIMEOUT = 3000

    def __init__(self, host, port = 7500):
        Thread.__init__(self)
        self.host = host
        self.port = port
        self.rlock = Lock()
        self.vlock = Lock()
        self.ready = True
        self.voltage = [0.0] * self.VCOUNT
        self.v_index = 0
        self.active = True
        self.nextRequest = None
        self.lastRequest = None

    def run(self):
        """main controlio loop"""
        self.setup()
        while self.active:
            with self.rlock:
                request = self.nextRequest
            if request is not None:
                if self.nextRequest != self.lastRequest:
                    self.ready = False
                    try:
                        self.socket.send(self.nextRequest)
                    except:
                        print("Error sending request")
                        continue
                    try:
                        reply = self.socket.recv()
                        with self.vlock:
                            self.voltage[self.v_index]  = struct.unpack("<f", reply)[0]
                            self.v_index = (self.v_index + 1) % self.VCOUNT
                        self.lastRequest = self.nextRequest
                        self.nextRequest = None
                    except:
                        print("Error in recv block")
                    self.ready = True
        self.cleanup()

    def setup(self):
        """Creates and connects socket for ZMQ i/o"""
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REQ)
        self.socket.RCVTIMEO = self.TIMEOUT
        self.socket.connect("tcp://%s:%d" % (self.host, self.port))

    def cleanup(self):
        """Closes socket and destroys zmq context"""
        self.socket.close()
        self.context.destroy()

    def send_command(self, clist):
        """Converts control settings to byte string and sends to rover"""
        with self.rlock:
            self.nextRequest = struct.pack("<iiii", *clist)

    def get_voltage(self):
        """Averages voltage numbers in voltage buffer"""
        with self.vlock:
            return sum(self.voltage) / self.VCOUNT

    def is_ready(self):
        """Returns True if thread is not currently sending command or waiting for response"""
        return self.ready
