# Copyright (c) 2022 Arm Limited. All rights reserved.

# Virtual Streaming Interface instance 0 Python script: Audio Input

##@addtogroup arm_vsi0_py_audio_in
#  @{
#
##@package arm_vsi0_audio_in
#Documentation for VSI Audio Input module.
#
#More details.

# NEAR MIC 2

import logging
import socket
import message as msg

SERVER=True
HOST = '127.0.0.1'    # The remote host
PORT = 50007              # The same port as used by the server

if SERVER:
   s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
   s.connect((HOST, PORT))
   # Identify as vht input number 2
   theBytes=msg.list_to_bytes(msg.clientID(msg.VSIINPUT,2))
   #print("vs0: %d %d" % (int(theBytes[0]),int(theBytes[1])))
   msg.sendBytes(s,theBytes)

## Set verbosity level
#verbosity = logging.DEBUG
verbosity = logging.ERROR

# [debugging] Verbosity settings
level = { 10: "DEBUG",  20: "INFO",  30: "WARNING",  40: "ERROR" }
logging.basicConfig(format='Py: VSI4: [%(levelname)s]\t%(message)s', level = verbosity)
logging.info("Verbosity level is set to " + level[verbosity])


# IRQ registers
IRQ_Status = 0

# Timer registers
Timer_Control  = 0
Timer_Interval = 0

# Timer Control register definitions
Timer_Control_Run_Msk      = 1<<0
Timer_Control_Periodic_Msk = 1<<1
Timer_Control_Trig_IRQ_Msk = 1<<2
Timer_Control_Trig_DMA_Msk = 1<<3

# DMA registers
DMA_Control = 0

# DMA Control register definitions
DMA_Control_Enable_Msk    = 1<<0
DMA_Control_Direction_Msk = 1<<1
DMA_Control_Direction_P2M = 0<<1
DMA_Control_Direction_M2P = 1<<1

# User registers
Regs = [0] * 64

CONTROL     = 0  # Regs[0]
CHANNELS    = 0  # Regs[1]
SAMPLE_BITS = 0  # Regs[2]
SAMPLE_RATE = 0  # Regs[3]

# User CONTROL register definitions
CONTROL_ENABLE_Msk = 1<<0


## Initialize
def init():
    logging.info("Python function init() called")


## Read interrupt request (the VSI IRQ Status Register)
#  @return value value read (32-bit)
def rdIRQ():
    global IRQ_Status
    #logging.info("Python function rdIRQ() called")

    value = IRQ_Status
    #logging.debug("Read interrupt request: {}".format(value))

    return value


## Write interrupt request (the VSI IRQ Status Register)
#  @param value value to write (32-bit)
#  @return value value written (32-bit)
def wrIRQ(value):
    global IRQ_Status
    #logging.info("Python function wrIRQ() called")

    IRQ_Status = value
    #logging.debug("Write interrupt request: {}".format(value))

    return value


## Write Timer registers (the VSI Timer Registers)
#  @param index Timer register index (zero based)
#  @param value value to write (32-bit)
#  @return value value written (32-bit)
def wrTimer(index, value):
    global Timer_Control, Timer_Interval
    #logging.info("Python function wrTimer() called")

    if   index == 0:
        Timer_Control = value
        #logging.debug("Write Timer_Control: {}".format(value))
    elif index == 1:
        Timer_Interval = value
        #logging.debug("Write Timer_Interval: {}".format(value))

    return value


## Timer event (called at Timer Overflow)
def timerEvent():
    pass
    #logging.info("Python function timerEvent() called")


## Write DMA registers (the VSI DMA Registers)
#  @param index DMA register index (zero based)
#  @param value value to write (32-bit)
#  @return value value written (32-bit)
def wrDMA(index, value):
    global DMA_Control
    #logging.info("Python function wrDMA() called")

    if   index == 0:
        DMA_Control = value
        #logging.debug("Write DMA_Control: {}".format(value))

    return value


## Read data from peripheral for DMA P2M transfer (VSI DMA)
#  @param size size of data to read (in bytes, multiple of 4)
#  @return data data read (bytearray)
def rdDataDMA(size):
    global s
    logging.info("Python function rdDataDMA() called : {}".format(size))
    logging.debug("Try to read data ({} bytes)".format(size))

    # Request RX data from server
    received=msg.receiveBytes(s,size)

    n = min(len(received), size)
    data = bytearray(size)
    for i in range(n):
        #data[i] = Data[i] + outdata[i]
        data[i] = received[i]
    
    return data


## Write data to peripheral for DMA M2P transfer (VSI DMA)
#  @param data data to write (bytearray)
#  @param size size of data to write (in bytes, multiple of 4)
def wrDataDMA(data, size):
    #logging.info("Python function wrDataDMA() called")

    #logging.debug("Write data ({} bytes)".format(size))

    return

## Write CONTROL register (user register)
#  @param value value to write (32-bit)
def wrCONTROL(value):
    global CONTROL
    if ((value ^ CONTROL) & CONTROL_ENABLE_Msk) != 0:
        if (value & CONTROL_ENABLE_Msk) != 0:
            pass
            #logging.info("Enable Receiver")
        else:
            pass
            #logging.info("Disable Receiver")
            

    CONTROL = value

## Write CHANNELS register (user register)
#  @param value value to write (32-bit)
def wrCHANNELS(value):
    global CHANNELS
    CHANNELS = value
    #logging.info("Number of channels: {}".format(value))

## Write SAMPLE_BITS register (user register)
#  @param value value to write (32-bit)
def wrSAMPLE_BITS(value):
    global SAMPLE_BITS
    SAMPLE_BITS = value
    #logging.info("Sample bits: {}".format(value))

## Write SAMPLE_RATE register (user register)
#  @param value value to write (32-bit)
def wrSAMPLE_RATE(value):
    global SAMPLE_RATE
    SAMPLE_RATE = value
    #logging.info("Sample rate: {}".format(value))


## Read user registers (the VSI User Registers)
#  @param index user register index (zero based)
#  @return value value read (32-bit)
def rdRegs(index):
    global Regs
    #logging.info("Python function rdRegs() called")

    value = Regs[index]
    #logging.debug("Read user register at index {}: {}".format(index, value))

    return value


## Write user registers (the VSI User Registers)
#  @param index user register index (zero based)
#  @param value value to write (32-bit)
#  @return value value written (32-bit)
def wrRegs(index, value):
    global Regs
    #logging.info("Python function wrRegs() called")

    if   index == 0:
        wrCONTROL(value)
    elif index == 1:
        wrCHANNELS(value)
    elif index == 2:
        wrSAMPLE_BITS(value)
    elif index == 3:
        wrSAMPLE_RATE(value)
    

    Regs[index] = value
    #logging.debug("Write user register at index {}: {}".format(index, value))

    return value


## @}

