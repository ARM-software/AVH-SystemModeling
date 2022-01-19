from multiprocessing import Process
import socket
import message as msg
import sys

print(sys.version_info)

HOST = '127.0.0.1'    # The remote host
PORT = 50007              # The same port as used by the server

NBIN=3 
NBOUT=2 

def connectToServer(inputMode,theid):
   s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
   s.connect((HOST, PORT))
   # Identify as vht input
   if inputMode:
      print("Connecting as INPUT")
      theBytes=msg.list_to_bytes(msg.clientID(msg.VSIINPUT,theid))
   else:
      print("Connecting as OUTPUT")
      theBytes=msg.list_to_bytes(msg.clientID(msg.VSIOUTPUT,theid))
   #print("vs0: %d %d" % (int(theBytes[0]),int(theBytes[1])))
   msg.sendBytes(s,theBytes)


if __name__ == '__main__':
    pIN=[None] * NBIN
    pOUT=[None] * NBOUT

    for i in range(NBIN):
       pIN[i] = Process(target=connectToServer, args=(True,i))
       pIN[i].start()

    for i in range(NBOUT):
      pOUT[i] = Process(target=connectToServer, args=(False,i))
      pOUT[i].start()
    
    print("Waiting for end")
    for i in range(NBIN):
      pIN[i].join()

    for i in range(NBOUT):
       pOUT[i].join()
    print("The End")