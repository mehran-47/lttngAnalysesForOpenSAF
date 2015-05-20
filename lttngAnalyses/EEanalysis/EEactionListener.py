#!/usr/bin/env python3
import sys, netifaces
from lttngAnalyses.networking.connection import connection
from multiprocessing import Process, Pipe
from subprocess import call

def printSent(child_pipe):
    while True:
        print(child_pipe.recv())

def listen_and_act(child_pipe):
    while True:
        latestInstruction = dict(child_pipe.recv())
        print(['EE.loc', latestInstruction.get('SI'), latestInstruction.get('action')])
        #call(['EE.loc', latestInstruction['SI'], latestInstruction['action']])


if __name__ == '__main__':
    server = connection(netifaces.ifaddresses('vmnet8')[2][0]['addr'], int(sys.argv[1]))
    print('server connection at %r:%r' %(netifaces.ifaddresses('vmnet8')[2][0]['addr'], sys.argv[1]))
    parent_c, child_c = Pipe()
    #Process(target=printSent, args=(child_c,)).start()
    Process(target=listen_and_act, args=(child_c,)).start()
    server.listen(parent_c)