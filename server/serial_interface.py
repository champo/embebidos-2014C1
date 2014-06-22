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

        self.__serial = serial.Serial(self.__port, self.__buad_rate)
# Arduino resets on connect
        sleep(1)

    def __clean(self):
        try:
            self.__serial.close()
        finally:
            self.__serial = None

    def get(self, address):
        with self.__lock:

            result = None
            while result is None:
                serial = self.serial()

                try:
                    serial.write([address, 1])
                    serial.flush()

                    result = serial.read(1)
                except:
                    self.__clean()

        return result



    def put(self, address, value):

        with self.__lock:

            result = None
            while result is None:
                serial = self.serial()

                try:
                    serial.write([address, 0, value])
                    serial.flush()

                    result = True

                except:
                    self.__clean()

