#!/usr/bin/env python3
import sys, netifaces, time
from lttngAnalyses.networking.connection import connection
from multiprocessing import Process, Pipe

def dispatch(IP, port, SI, action):
    client = connection(netifaces.ifaddresses('eth0')[2][0]['addr'], 5555)
    client.connect(IP, port)
    time.sleep(1)
    client.send({'SI':SI, 'action':action})


if __name__ == '__main__':
    client = connection('172.16.198.30', int(sys.argv[2]))
    client.connect(sys.argv[1], int(sys.argv[2]))
    #print('client connection at %r:%r' %(netifaces.ifaddresses('vmnet8')[2][0]['addr'], sys.argv[1]))
    time.sleep(1)
    client.send({'SI':'SI:some_SI_name_2', 'action':'increase'})
    time.sleep(2)
    client.send({'SI':'SI:some_SI_name_1', 'action':'decrease'})
    time.sleep(1)
    client.send({'SI':'SI:some_SI_name', 'action':'increase'})