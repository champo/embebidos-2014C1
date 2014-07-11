import serial
import struct
import sys
import time

from config import DEVICE
SPEED = 9600

def main(args):
    if len(args) != 2:
        return usage()
    i2c_address = int(args[0])
    type = int(args[1])

    ser = serial.Serial()
    ser.port = DEVICE
    ser.baudrate = SPEED
    ser.open()

# Wait for the arduino to reset
    print "Sleep"
    time.sleep(2)

    ser.write([i2c_address])
    ser.write([1])
    ser.write([type])

    print "Flush"
    ser.flush()

    print "Reading..."
    out = ser.read(1)
    print '%d' % (struct.unpack('B', out)[0])
    ser.close()

def usage():
    print ('Usage: \n\t%s i2c_address type\n\t'
        'Send a get to the module on the specified i2c address.\n'
        'Types: Luminosity (1)\n'
        '\tBlinds (2)\n'
        '\tAir conditioner (3)\n'
        '\tTemperature (4)\n'
        '\tLamp (5)\n')

if __name__ == '__main__':
    main(sys.argv[1:])
