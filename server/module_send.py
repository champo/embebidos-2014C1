import serial
import struct
import sys
import time

DEVICE = '/dev/tty.usbmodem1421'
SPEED = 9600

def main(args):
    if len(args) != 2:
        return usage()
    i2c_address = int(args[0])
    value_to_send = int(args[1])

    ser = serial.Serial()
    ser.port = DEVICE
    ser.baudrate = SPEED
    ser.open()

# Wait for the arduino to reset
    time.sleep(2)

    ser.write([i2c_address])
    ser.write([0])
    ser.write([value_to_send])

    ser.flush()
    ser.close()

def usage():
    print ('Usage: \n\t%s i2c_address value\n\t'
        'Send a PUT with the given value to the module on the specified i2c address.')

if __name__ == '__main__':
    main(sys.argv[1:])
