import serial
import sys

DEVICE = '/dev/tty.SLAB_USBtoUART'
SPEED = 9600

def main(args):
    if len(args) != 1:
        return usage()
    i2c_address = args[0]
    ser = serial.open(DEVICE, SPEED, timeout = 1)
    ser.write() # TODO: What should I write?
    ser.flush()
    print '%d' % ser.read(1)
    ser.close()

def usage():
    print ('Usage: \n\t%s i2c_address\n\t'
        'Send a get to the module on the specified i2c address.')

if __name__ == '__main__':
    main(sys.argv[1:])
