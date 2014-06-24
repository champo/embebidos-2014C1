import serial
import struct
import sys
import time

DEVICE = '/dev/tty.usbmodem1421'
SPEED = 9600

def main(args):
    if len(args) != 1:
        return usage()
    i2c_address = int(args[0])

    ser = serial.Serial()
    ser.port = DEVICE
    ser.baudrate = SPEED
    ser.open()

# Wait for the arduino to reset
    print "Sleep"
    time.sleep(2)

    ser.write([i2c_address])
    ser.write([1])

    print "Flush"
    ser.flush()

    print "Reading..."
    out = ser.read(1)
    print '%d' % (struct.unpack('B', out)[0])
    ser.close()

def usage():
    print ('Usage: \n\t%s i2c_address\n\t'
        'Send a get to the module on the specified i2c address.')

if __name__ == '__main__':
    main(sys.argv[1:])
