import numpy as np
import sys


def ParserMessgae(message):
    A = np.zeros((3, 1), float)
    w = np.zeros((3, 1), float)
    A[0] = float(message[14:26])
    A[1] = float(message[28:41])
    A[2] = float(message[44:56])

    w[0] = 1 / (float(message[67:73]) * float(message[67:73]))
    w[1] = 1 / (float(message[76:82]) * float(message[76:82]))
    w[2] = 1 / (float(message[85:91]) * float(message[85:91]))

    return A, w


def DataAver(A, w):
    dataAv = []
    dataAv.append(np.average(A, weights=w))
    return dataAv

def SolutionStatus(AAA, PPP, RTK):

    for i in AAA:
        for j in PPP:
            for k in RTK:
                if i < j:
                    sys.stdout.write(f"\r{format('PPP')}")
                    sys.stdout.flush()
                elif i > j:
                    sys.stdout.write(f"\r{format('ABS')}")
                    sys.stdout.flush()
                elif k < j:
                    sys.stdout.write(f"\r{format('RTK')}")
                    sys.stdout.flush()
                else:
                    sys.stdout.write(f"\r{format('Not solution.')}")
                    sys.stdout.flush()