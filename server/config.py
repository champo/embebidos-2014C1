import os

_basedir = os.path.abspath(os.path.dirname(__file__))

DEBUG = True

DEVICE = '/dev/tty.usbmodem1411'

SECRET_KEY = 'ophwgenp8rgbpuoaenpoirgnjaeopnjgpoaf'

SQLALCHEMY_DATABASE_URI = 'sqlite:///' + os.path.join(_basedir, 'test.db')
DATABASE_CONNECT_OPTIONS = {}

THREADS_PER_PAGE = 8

CSRF_ENABLED = True
CSRF_SESSION_KEY = "somethingimpossibletoguess"
