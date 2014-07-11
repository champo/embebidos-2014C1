import serial
import sys
import time

from config import DEVICE
SPEED = 9600

def main(args):
    if len(args) != 3:
        return usage()
    i2c_address = int(args[0])
    type = int(args[1])
    value_to_send = int(args[2])

    ser = serial.Serial()
    ser.port = DEVICE
    ser.baudrate = SPEED
    ser.open()

# Wait for the arduino to reset
    time.sleep(2)

    ser.write([i2c_address])
    ser.write([0])
    ser.write([type])
    ser.write([value_to_send])

    ser.flush()
    ser.close()

def usage():
    print ('Usage: \n\t%s i2c_address type value\n\t'
        'Send a PUT with the given value to the module on the specified i2c address.'
        '\nTypes: Luminosity (1), \n'
        '\tBlinds (2)\n'
        '\tAir conditioner (3)\n'
        '\tTemperature (4)\n'
        '\tLamp (5)\n')

if __name__ == '__main__':
    main(sys.argv[1:])
