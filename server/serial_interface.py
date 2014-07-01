import serial
import struct

from threading import RLock
from time import sleep

class SerialInterface(object):

    def __init__(self, port, baud_rate):
        self.__port = port
        self.__buad_rate = baud_rate

        self.__lock = RLock()
        self.__taken = False

        self.__serial = None

    def __enter__(self):
        self.__lock.acquire()

    def __exit__(self, type, value, traceback):
        self.__lock.release()

    def serial(self):
        if self.__serial is not None:
            return self.__serial

        print "New serial"
        self.__serial = serial.Serial(self.__port, self.__buad_rate)
# Arduino resets on connect
        sleep(2)
        return self.__serial

    def __clean(self):
        try:
            print "__clean"
            self.__serial.close()
        finally:
            self.__serial = None

    def get(self, address, type):
        with self.__lock:

            result = None
            while result is None:
                serial = self.serial()

                try:
                    serial.write([address, 1, type])
                    serial.flush()

                    result = struct.unpack('B', serial.read(1))[0]
                except Exception as e:
                    print e
                    self.__clean()

        return result

    def put(self, address, type, value):

        with self.__lock:

            result = None
            while result is None:
                serial = self.serial()

                try:
                    serial.write([address, 0, type, int(value)])
                    serial.flush()

                    result = True

                except Exception as e:
                    print e
                    self.__clean()

