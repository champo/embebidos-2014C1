import serial
import struct
import sys

DEVICE = '/dev/tty.SLAB_USBtoUART'
SPEED = 9600

def main(args):
    if len(args) != 1:
        return usage()
    i2c_address = int(args[0])
    ser = serial.Serial(DEVICE, SPEED, timeout = 1)
    ser.write(struct.pack('BB', i2c_address, 0))
    ser.flush()
    print '%d' % (struct.unpack('B', ser.read(1))[0])
    ser.close()

def usage():
    print ('Usage: \n\t%s i2c_address\n\t'
        'Send a get to the module on the specified i2c address.')

if __name__ == '__main__':
    main(sys.argv[1:])
