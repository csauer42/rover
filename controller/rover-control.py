#!/usr/bin/python

from rover import Rover
import sys

def main():
    if len(sys.argv) != 2:
        print("Usage: ./rover-control.py [ip address]")
        sys.exit(1)

    ip = sys.argv[1]
    rover = Rover(ip)
    rover.run()

if __name__ == '__main__':
    main()

