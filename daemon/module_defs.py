"""Module name to identifier mapping"""

from enum import Enum

class Module(Enum):
    """Module name to identifier"""
    CONFIG = 0
    CLOCK = 1
    DEVICE = 2
    IO = 3
    LED = 4
    MOUSE = 5
    COMMAND = 6
    CLIENT = 7
    NOTIFY = 8
    KEYBOARD_LAYOUT = 9

class LogLevel(Enum):
    """Map log level names to pinelog levels"""
    # This is hard coded to the pinelog levels
    NOTSET = -2
    NONE = -1
    FATAL = 0
    ERROR = 1
    WARNING = 2
    INFO = 3
    DEBUG = 4
    TRACE = 5
