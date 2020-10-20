from functools import reduce
import socket

serverSocket = socket.socket()

def RequestRTK(AproxPos):
    serverSocket.connect(('192.168.1.34', 5000))
    serverSocket.send(b'tcpcli://192.168.1.44:22 rtcm3')
    serverSocket.send(AproxPos)
    serverSocket.close()


def RTKConfCreator(inpstrType, inpstrPath, outstrType, outstrPath, outstrType2, outstrPath2):
    confFile = '~/RTKLIB/app/rtkrcv/gcc/confFile.conf'
    rtkConf = 'rtk.conf'
    data_file = {"inpstr1-type       =": f"inpstr1-type       ={inpstrType}",
                 "inpstr1-path       =": f"inpstr1-path       ={inpstrPath}",
                 "outstr1-type       =": f"outstr1-type       ={outstrType}",
                 "outstr1-path       =": f"outstr1-path       ={outstrPath}",
                 "outstr2-type       =": f"outstr2-type       ={outstrType2}",
                 "outstr2-path       =": f"outstr2-path       ={outstrPath2}"
                 }
    with open(confFile, 'r') as readFile:
        olddata = readFile.read()

        new_data = reduce(lambda a, kv: a.replace(*kv), data_file.items(), olddata)

    with open(rtkConf, 'w') as f:
        f.write(new_data)
