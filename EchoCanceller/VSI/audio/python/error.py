import socket

ERROROCCURED=False

def sockErrorRead(func):
    def wrapper(size):
        global ERROROCCURED
        if ERROROCCURED:
            return(bytearray(size))
        try:
            data=func(size)
            return(data)
        except Exception as e:
            ERROROCCURED=True
            return(bytearray(size))
        
    return wrapper

def sockErrorWrite(func):
    def wrapper(data,size):
        global ERROROCCURED
        if ERROROCCURED:
            return
        try:
            func(data,size)
        except Exception as e:
            ERROROCCURED=True
        
    return wrapper

def sockErrorWriteValue(func):
    def wrapper(value):
        global ERROROCCURED
        if ERROROCCURED:
            return
        try:
            func(value)
        except Exception as e:
            ERROROCCURED=True
        
    return wrapper