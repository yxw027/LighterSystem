import subprocess
import threading
import time
import gnsscal
import datetime
from BoardController.AnalysisModule import *
from BoardController.RTKSolver import *


eventConnection = threading.Event()
eventForSynthesis = threading.Event()
eventForRTK = threading.Event()

storageABS = []
storagePPP = []
storageRTK = []

ABS = socket.socket()
PPP = socket.socket()
RTK = socket.socket()

ABS.bind(('127.0.0.1', 8090))
ABS.listen(1)

PPP.bind(('127.0.0.1', 8080))
PPP.listen(1)

RTK.bind(('127.0.0.1', 8070))
RTK.listen(1)

conn = []
solver = []
AproxPos = []

def ReciveRTK():
    eventForRTK.wait()
    RTKstor = []

    while True:
        try:
            dataRTK = conn['RTK'].recv(1024)
            if dataRTK:
                storageRTK.append(dataRTK.decode('utf-8'))
                for i in storageRTK[:]:
                    streamRTK = i.split('\n')
                    for sRTK in streamRTK:
                        if str(GPSweek[0]) in sRTK:
                            if len(sRTK) == 132:
                                Artk, Wrtk = ParserMessgae(sRTK)
                                if Wrtk.all() < 0.5:
                                    eventConnection.set()
                            else:
                                continue

        except socket.error as e:
            if e.errno == 10053:
                conn.pop(i)
            else:
                raise



def Reciver():
    eventConnection.wait()
    ABSstor =[]
    PPPstor = []

    while True:
        for i in range(len(conn)):
            try:
                dataABS = conn['ABS'].recv(1024)
                dataPPP = conn['PPP'].recv(1024)
                if dataABS or dataPPP:
                    AproxPos.append(dataABS)
                    storageABS.append(dataABS.decode('utf-8'))
                    storagePPP.append(dataPPP.decode('utf-8'))
                    for i in storageABS[:]:
                        for j in storagePPP[:]:
                            streamABS = i.split('\n')
                            streamPPP = j.split('\n')
                            for sABS in streamABS:
                                for sPPP in streamPPP:
                                    if str(GPSweek[0]) in sABS and sPPP:
                                        if len(sABS) and len(sPPP) == 132:
                                            Aabs, Wabs = ParserMessgae(sABS)
                                            ABSstor.append(DataAver(Aabs, Wabs))
                                            Appp, Wppp = ParserMessgae(sPPP)
                                            if Wppp.all() >= 0.5:
                                                subprocess.call(['nohup ~/RTKLIB/app/rtkrcv/gcc/rtkrcvRTK 2> exhibitor.out &'])
                                                eventConnection.clear()
                                                eventForRTK.set()
                                            else:
                                                eventForRTK.clear()
                                                PPPstor.append(DataAver(Appp, Wppp))
                                                SolutionStatus(ABSstor, PPPstor)
                                        else:
                                            continue

            except socket.error as e:
                if e.errno == 10053:
                    conn.pop(i)
                else:
                    raise



def Accepter():
    while 1:
        global conn
        if ABS.accept()[0] or PPP.accept()[0] or RTK.accept()[0]:
            conn = {'ABS' : ABS.accept()[0],'PPP' : PPP.accept()[0], 'RTK' : RTK.accept()[0]}
        eventConnection.set()




today = datetime.date.today()
GPSweek = gnsscal.date2gpswd(today)

# init threads
AccepterConnection = threading.Thread(target=Accepter)
Recivers = threading.Thread(target=Reciver)
RTKOptimization = threading.Thread(target=ReciveRTK)

# start threads
AccepterConnection.start()
Recivers.start()
RTKOptimization.start()


def main():
    time.sleep(10)
    RequestRTK(AproxPos)


if __name__ == '__main__':
    RTKConfCreator("tcpcli", "Dolin:SergeySGGABG12@igs-ip.net:2101/NRC100CAN0", "tcpcli","192.168.1.44:5000", "tcpcli", "127.0.0.1:8070")
