#!/usr/bin/env python3
import sys, netifaces, time
from lttngAnalyses.networking.connection import connection
from multiprocessing import Process, Pipe


if __name__ == '__main__':
    client = connection('172.16.198.30', int(sys.argv[2]))
    client.connect(sys.argv[1], int(sys.argv[2]))
    #print('client connection at %r:%r' %(netifaces.ifaddresses('vmnet8')[2][0]['addr'], sys.argv[1]))
    time.sleep(1)
    client.send({'msg':'please?'})
    time.sleep(2)
    client.send({'msg':'pretty please?'})
    time.sleep(1)
    client.send({'msg':'whaa?'})