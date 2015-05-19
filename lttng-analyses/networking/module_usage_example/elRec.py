#!/usr/bin/env python3
import sys, netifaces
from networking.connection import connection
from multiprocessing import Process, Pipe

def printSent(child_pipe):
    while True:
        print(child_pipe.recv())

if __name__ == '__main__':
    server = connection(netifaces.ifaddresses('vmnet8')[2][0]['addr'], int(sys.argv[1]), debug=True)
    print('server connection at %r:%r' %(netifaces.ifaddresses('vmnet8')[2][0]['addr'], sys.argv[1]))
    parent_c, child_c = Pipe()
    Process(target=printSent, args=(child_c,)).start()
    server.listen(parent_c)