#!/usr/bin/env python3
import threading
import time
import socket
import sys
import os

#Function for handling connections. This will be used to create threads
def clientthread(host, port):
    HOST = host         # Symbolic name meaning all available interfaces
    PORT = port    # Arbitrary non-privileged port
    msg = "connection error"
     
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print('Socket created')
     
    try:
        s.bind((HOST, 5001))
    except (socket.error, msg):
        print('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
        sys.exit()
         
    print('Socket bind complete')

    ipname = 'Connection made from ' + socket.gethostbyname(socket.gethostname()) + ':5001'
    s.connect((host , port))
    s.send((ipname).encode('utf-8')) #send only takes string
    for z in range(5):
        reply = "\n"+str(z)
        time.sleep(2)
        s.sendall(reply.encode('utf-8'))
    s.close()


clientthread(sys.argv[1],int(sys.argv[2]))