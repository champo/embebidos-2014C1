
import serial
import sys

DEVICE = '/dev/tty.SLAB_USBtoUART'
SPEED = 9600

def main(args):
    if len(args) != 2:
        return usage()
    i2c_address = args[0]
    value_to_send = args[1]
    ser = serial.open(DEVICE, SPEED, timeout = 1)
    ser.write() # TODO: What should I write?
    ser.flush()
    ser.close()

def usage():
    print ('Usage: \n\t%s i2c_address value\n\t'
        'Send a get to the module on the specified i2c address.')

if __name__ == '__main__':
    main(sys.argv[1:])
