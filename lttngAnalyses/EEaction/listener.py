#!/usr/bin/env python3
import sys, netifaces as ni
from lttngAnalyses.networking.connection import connection
from multiprocessing import Process, Pipe
from subprocess import call

def printSent(child_pipe):
    while True:
        print(child_pipe.recv())

def listen():
    server = connection(ni.ifaddresses(sys.argv[1])[2][0]['addr'], int(sys.argv[2]))
    print('Listening for EE action at %r:%r' %(ni.ifaddresses(sys.argv[1])[2][0]['addr'], sys.argv[2]))
    parent_c, child_c = Pipe()
    #Process(target=printSent, args=(child_c,)).start()
    Process(target=act, args=(child_c,)).start()
    server.listen(parent_c)

def act(child_pipe):
    while True:
        latestInstruction = dict(child_pipe.recv())
        #print(['/opt/bin/ElasticityEngineCMD', latestInstruction.get('SI'), latestInstruction.get('action')])
        #call(['/home/node1/Downloads/lttngAnalysesForOpenSAF/EE_tool_by_neha/Neha_EETool/ElasticityEngineCMD', latestInstruction.get('SI'), latestInstruction.get('action')])
        if latestInstruction.get('SI'):
            print(['/opt/bin/ElasticityEngineCMD', latestInstruction.get('SI'), latestInstruction.get('action')])
            call(['/opt/bin/ElasticityEngineCMD', latestInstruction.get('SI'), latestInstruction.get('action')])

if __name__ == '__main__':
    if sys.argv[2:]: 
        listen()
    else:
        print('Usage: "python3 -m lttngAnalyses.EEaction.listener <interface> <port>"\n\
            e.g. "python3 -m lttngAnalyses.EEaction.listener eth0 4444"')